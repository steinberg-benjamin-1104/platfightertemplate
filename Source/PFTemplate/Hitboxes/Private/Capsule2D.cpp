#include "Capsule2D.h"
#include "Procedural2DCapsuleComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "FighterPawn.h"

ACapsule2D::ACapsule2D()
{
    PrimaryActorTick.bCanEverTick = false;

    CapsuleMesh = CreateDefaultSubobject<UProcedural2DCapsuleComponent>(TEXT("ProceduralCapsule"));
    RootComponent = CapsuleMesh;

    CollisionCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionCapsule"));
    CollisionCapsule->SetupAttachment(RootComponent);
    CollisionCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    CollisionCapsule->SetGenerateOverlapEvents(true);
}

void ACapsule2D::Initialize(AFighterPawn* InOwner)
{
    FighterPawnRef = InOwner;
    if (FighterPawnRef)
    {
        SetOwner(FighterPawnRef);
    }
}

void ACapsule2D::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    SetCapsuleSize(CapsuleSize);
}

#if WITH_EDITOR
void ACapsule2D::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.Property &&
        PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(ACapsule2D, CapsuleSize))
    {
        OnConstruction(GetActorTransform());
    }
}
#endif

void ACapsule2D::SetDebugVisible(bool bVisible)
{
    if (CapsuleMesh)
    {
        CapsuleMesh->SetVisibility(bVisible, false);
        CapsuleMesh->SetHiddenInGame(!bVisible, false);
    }
}

void ACapsule2D::SetCapsuleSize(FVector2D NewCapsuleSize)
{
    CapsuleSize = NewCapsuleSize;
    
    if (CapsuleMesh)
    {
        CapsuleMesh->CapsuleSize = NewCapsuleSize;
        CapsuleMesh->RebuildCapsule();
    }

    if (CollisionCapsule)
    {
        float Radius = NewCapsuleSize.X;
        float HalfHeight = NewCapsuleSize.Y + NewCapsuleSize.X;

        CollisionCapsule->SetCapsuleRadius(Radius);
        CollisionCapsule->SetCapsuleHalfHeight(HalfHeight);
        
        CollisionCapsule->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
    }
}