#include "PFCamera.h"
#include "Engine/World.h"
#include "CamTarget.h"

APFCamera::APFCamera()
{
    PrimaryActorTick.bCanEverTick = false;

    RootMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RootMesh"));
    RootMesh->SetHiddenInGame(true);
    RootMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    RootComponent = RootMesh;
    RootMesh->SetRelativeRotation(FRotator(BasePitch, BaseYaw, 0.f));
    
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootMesh);
    SpringArm->TargetArmLength = MinArmLength;
    SpringArm->bDoCollisionTest = false;
    SpringArm->bEnableCameraLag = false;

    CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
    CameraComp->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
    CameraComp->bUsePawnControlRotation = false;
}

void APFCamera::AddTargetActor(AActor* Actor)
{
    if (!Actor) return;

    if (Actor->GetClass()->ImplementsInterface(UCamTarget::StaticClass()))
    {
        TScriptInterface<ICamTarget> TargetInterface;
        TargetInterface.SetObject(Actor);
        TargetInterface.SetInterface(Cast<ICamTarget>(Actor));

        CamTargets.AddUnique(TargetInterface);
    }
}

void APFCamera::RemoveTargetActor(AActor* Actor)
{
    if (!Actor) return;
    
    TScriptInterface<ICamTarget> TargetInterface;
    TargetInterface.SetObject(Actor);
    TargetInterface.SetInterface(Cast<ICamTarget>(Actor));
    CamTargets.Remove(TargetInterface);
}

void APFCamera::UpdateCamera()
{
    if (CamTargets.Num() == 0 || !SpringArm || !CameraComp) return;

    const FBoxBounds TargetBounds = CalcTargetBounds();

    UpdateCameraZoom(TargetBounds);
    UpdateCameraLocation(TargetBounds);
    // DrawDebugBounds(CalcCameraProjectionBounds(GetActorLocation()));
    // DrawDebugBounds(CameraBounds);
}

void APFCamera::UpdateCameraLocation(const FBoxBounds& TargetBounds)
{
    // Step 1: Desired camera center from fighters
    FVector Desired(
        (TargetBounds.Left + TargetBounds.Right) * 0.5f,
        0.f,
        (TargetBounds.Bottom + TargetBounds.Top) * 0.5f
    );

    // Smoothly move toward desired center
    FVector NewLoc = FMath::VInterpTo(GetActorLocation(), Desired, FixedFrameTime, PanSpeed);

    // Step 2: Adjust so projection box stays inside CameraBounds
    FBoxBounds Proj = CalcCameraProjectionBounds(NewLoc);

    // Compute offsets needed if projection goes past stage bounds
    float OffsetX = 0.f;
    float OffsetZ = 0.f;

    if (Proj.Left < CameraBounds.Left)
        OffsetX = CameraBounds.Left - Proj.Left;
    else if (Proj.Right > CameraBounds.Right)
        OffsetX = CameraBounds.Right - Proj.Right;

    if (Proj.Bottom < CameraBounds.Bottom)
        OffsetZ = CameraBounds.Bottom - Proj.Bottom;
    else if (Proj.Top > CameraBounds.Top)
        OffsetZ = CameraBounds.Top - Proj.Top;

    // Apply correction so extents "touch" but never exceed the bounds
    NewLoc.X += OffsetX;
    NewLoc.Z += OffsetZ;

    SetActorLocation(NewLoc);
}

void APFCamera::UpdateCameraZoom(const FBoxBounds& TargetBounds)
{
    // Span of fighters in world space
    float PlayerWidth  = TargetBounds.Right - TargetBounds.Left;
    float PlayerHeight = TargetBounds.Top   - TargetBounds.Bottom;

    PlayerWidth  = FMath::Max(PlayerWidth,  1.f);
    PlayerHeight = FMath::Max(PlayerHeight, 1.f);

    // Horizontal and vertical FOV (in radians)
    float HFOVRad = FMath::DegreesToRadians(CameraComp->FieldOfView);
    float VFOVRad = 2.f * FMath::Atan(FMath::Tan(HFOVRad * 0.5f) / CameraComp->AspectRatio);

    // Arm length needed to fit width
    float ArmFromWidth  = PlayerWidth  / (2.f * FMath::Tan(HFOVRad * 0.5f));
    // Arm length needed to fit height
    float ArmFromHeight = PlayerHeight / (2.f * FMath::Tan(VFOVRad * 0.5f));

    // Take the larger so both dimensions fit
    float RequiredArm = FMath::Max(ArmFromWidth, ArmFromHeight);

    // Clamp to min/max
    float TargetArm = FMath::Clamp(RequiredArm, MinArmLength, MaxArmLength);

    // Smooth transition
    SpringArm->TargetArmLength = FMath::FInterpTo(
        SpringArm->TargetArmLength,
        TargetArm,
        FixedFrameTime,
        ZoomSpeed
    );

    // Debug: compare spans vs. actual camera projection
    float HalfW, HalfH;
    CalcHalfProjection(TargetArm, HalfW, HalfH);
    // UE_LOG(LogTemp, Warning,
    //     TEXT("PlayerW=%.1f | PlayerH=%.1f | CamW=%.1f | CamH=%.1f | Arm=%.1f"),
    //     PlayerWidth, PlayerHeight,
    //     HalfW*2.f, HalfH*2.f,
    //     TargetArm);
}

FBoxBounds APFCamera::CalcTargetBounds() const
{
    //Initializes the box with extreme values so the first real target will overwrite them
    FBoxBounds B { FLT_MAX, -FLT_MAX, FLT_MAX, -FLT_MAX };

    for (TScriptInterface<ICamTarget> T : CamTargets)
    {
        if (!T) continue;

        FVector L = ClampLocationToBounds(T->CenterLocation());
        //UE_LOG(LogTemp, Warning, TEXT("Target L: X=%f, Z=%f"), L.X, L.Z);

        B.Left   = FMath::Min(B.Left,   L.X);
        B.Right  = FMath::Max(B.Right,  L.X);
        B.Bottom = FMath::Min(B.Bottom, L.Z);
        B.Top    = FMath::Max(B.Top,    L.Z);
    }

    if (CamTargets.Num() == 0)
        B = {0,0,0,0};

    B.Top += CeilingPadding;
    B.Bottom -= Padding;
    B.Left -= Padding;
    B.Right += Padding;

    return B;
}

FBoxBounds APFCamera::CalcCameraProjectionBounds(FVector L) const
{
    // Figure out how far the camera can see
    float HalfW, HalfH;
    CalcHalfProjection(SpringArm->TargetArmLength, HalfW, HalfH);

    // Build a 2D bounding box (XZ plane) of what the camera currently sees:
    return FBoxBounds(L.X - HalfW, L.X + HalfW, L.Z - HalfH, L.Z + HalfH);
}

void APFCamera::CalcHalfProjection(float Dist, float& OutHalfW, float& OutHalfH) const
{
    // Convert horizontal field of view (in degrees) to radians
    const float HFOVRad = FMath::DegreesToRadians(CameraComp->FieldOfView * 0.5f);
    // Half width = Distance * tan(half-FOV)
    OutHalfW = Dist * FMath::Tan(HFOVRad);
    // So HalfHeight = HalfWidth / AspectRatio
    OutHalfH = OutHalfW / CameraComp->AspectRatio;
}

FVector APFCamera::ClampLocationToBounds(const FVector& Loc) const
{
    return FVector(
        FMath::Clamp(Loc.X, CameraBounds.Left, CameraBounds.Right),
        Loc.Y,
        FMath::Clamp(Loc.Z, CameraBounds.Bottom, CameraBounds.Top)
    );
}

void APFCamera::DrawDebugBounds(const FBoxBounds& Bounds) const
{
    UWorld* World = GetWorld();
    if (!World) return;

    const FVector TopLeft     = FVector(Bounds.Left, 0.f, Bounds.Top);
    const FVector TopRight    = FVector(Bounds.Right, 0.f, Bounds.Top);
    const FVector BottomLeft  = FVector(Bounds.Left, 0.f, Bounds.Bottom);
    const FVector BottomRight = FVector(Bounds.Right, 0.f, Bounds.Bottom);

    const FColor Color = FColor::Blue;
    const bool bPersistent = false;
    const float Lifetime = 0.f; // one frame
    const float thickness = 5.f;

    DrawDebugLine(World, TopLeft, TopRight, Color, bPersistent, Lifetime, 0, thickness);
    DrawDebugLine(World, TopRight, BottomRight, Color, bPersistent, Lifetime, 0, thickness);
    DrawDebugLine(World, BottomRight, BottomLeft, Color, bPersistent, Lifetime, 0, thickness);
    DrawDebugLine(World, BottomLeft, TopLeft, Color, bPersistent, Lifetime, 0, thickness);
}