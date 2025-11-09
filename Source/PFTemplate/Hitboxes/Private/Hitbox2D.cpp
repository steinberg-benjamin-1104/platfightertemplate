#include "Hitbox2D.h"
#include "FighterPawn.h"
#include "Hurtbox2D.h"
#include "Procedural2DCapsuleComponent.h"
#include "Components/CapsuleComponent.h"

AHitbox2D::AHitbox2D()
{
    bIsActive = false;
    PrimaryActorTick.bCanEverTick = false;
    SetActorTickEnabled(false);
}

void AHitbox2D::BeginPlay()
{
    Super::BeginPlay();
    SetBoxActive(false, FHitboxDefinition());
}

void AHitbox2D::SetBoxActive(bool bActivate, const FHitboxDefinition& InDefinition)
{
    Super::SetBoxActive(bActivate, InDefinition);

    if (bIsActive)
    {
        HitboxDefinition = InDefinition;
        UpdateTransform();
    }

    SetDebugVisible(bIsActive);
}

void AHitbox2D::TickHitbox()
{
    if (HitboxDefinition.Transform.bIsAttached && bIsActive) UpdateLocation();
}

void AHitbox2D::UpdateTransform()
{
    if (!FighterPawnRef) return;

    ApplyMaterialForType();
    SetCapsuleSize(HitboxDefinition.Transform.Size);
    UpdateLocation();
    UpdateRotation();
}

void AHitbox2D::UpdateLocation()
{
    if (!FighterPawnRef) return;
    
    const FVector BoneWorldLocation = FighterPawnRef->GetBoneLocation(HitboxDefinition.Transform.BoneName);
    const FVector2D Offset2D = HitboxDefinition.Transform.LocationFromBone;
    
    const FVector Offset3D(Offset2D.X * FighterPawnRef->GetFacingDirection(), 0.f, Offset2D.Y);

    const FVector FinalLocation = BoneWorldLocation + Offset3D;
    SetActorLocation(FinalLocation);
}

void AHitbox2D::UpdateRotation()
{
    float Pitch = HitboxDefinition.Transform.Rotation;
    if (!FighterPawnRef->IsFacingRight())
    {
        Pitch = 180.f - Pitch;
    }

    SetActorRotation(FRotator(Pitch, 0.f, 0.f));
}

void AHitbox2D::ApplyMaterialForType()
{
    if (!CapsuleMesh) return;

    EHitboxType HitboxType = HitboxDefinition.Transform.HitboxType;
    if (const FHitboxMaterialSet* MaterialSet = MaterialSets.Find(HitboxType))
    {
        CapsuleMesh->SetMaterialSet(*MaterialSet);
    }
}

void AHitbox2D::GetHitPlayers(TArray<FPendingHit>& OutHits, TArray<TScriptInterface<IHittable>>& AlreadyHit) const
{
    if (!bIsActive) return;

    TArray<UPrimitiveComponent*> OverlappingComps;
    CollisionCapsule->GetOverlappingComponents(OverlappingComps);

    for (UPrimitiveComponent* Comp : OverlappingComps)
    {
        const AHurtbox2D* Hurtbox = Cast<AHurtbox2D>(Comp->GetOwner());
        if (!Hurtbox) continue;

        TScriptInterface<IHittable> Hittable(Hurtbox->GetOwner());
        if (Hurtbox->IsInvulnerable() || AlreadyHit.Contains(Hittable) ||
            Hurtbox->GetOwner() == FighterPawnRef ||
            !Hurtbox->GetOwner()->GetClass()->ImplementsInterface(UHittable::StaticClass()))
            continue;

        AlreadyHit.Add(Hittable);
        OutHits.Emplace(Hittable, GetDamageInfo());
    }
}