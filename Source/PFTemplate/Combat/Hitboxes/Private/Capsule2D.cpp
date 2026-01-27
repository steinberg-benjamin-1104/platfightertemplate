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

void ACapsule2D::SetCapsuleSize(FFixedVector2D NewCapsuleSize)
{
    CapsuleSize = NewCapsuleSize;
    
    if (CapsuleMesh) CapsuleMesh->SetCapsuleSize(Fixed2DToVector2D(NewCapsuleSize));
    
    CollisionCapsule.Capsule.SetSize(NewCapsuleSize);
    
}

void ACapsule2D::SetCapsuleRotation(FFixed_32 Rotation)
{
    CollisionCapsule.Capsule.SetRotation(Rotation);
    
    //For procedural capsule visual, doesn't affect collision
    SetActorRotation(FRotator(FixedToFloat(Rotation), 0.f, 0.f));
}

void ACapsule2D::SetCapsuleLocation(FFixedVector2D NewLoc)
{
    SetActorLocation(Fixed2DToVector(NewLoc));

    CollisionCapsule.WorldPos = NewLoc;
}