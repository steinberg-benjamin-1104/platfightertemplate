#include "Hurtbox2DComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "FighterPawn.h"

UHurtbox2DComponent::UHurtbox2DComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) // calls UProcedural2DCapsuleComponent(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UHurtbox2DComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!TargetSkeletalMesh && FighterPawnRef)
	{
		TargetSkeletalMesh = FighterPawnRef->FindComponentByClass<USkeletalMeshComponent>();
	}
}

void UHurtbox2DComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bHurtboxEnabled)
	{
		UpdateFromBone();
	}
}

void UHurtbox2DComponent::ToggleHurtboxActive(bool bActive)
{
	bHurtboxEnabled = bActive;
	SetHiddenInGame(!bActive);
	SetCollisionEnabled(bActive ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
}

void UHurtbox2DComponent::UpdateFromBone()
{
	if (!TargetSkeletalMesh || !FighterPawnRef || TargetBoneName.IsNone())
	{
		return;
	}

	FVector BoneWorldLocation = TargetSkeletalMesh->GetSocketLocation(TargetBoneName);

	// Lock to XZ plane
	BoneWorldLocation.Y = 0.f;

	// Apply facing offset
	const float Dir = FighterPawnRef->IsFacingRight() ? 1.f : -1.f;
	const FVector Offset3D(XZOffset.X * Dir, 0.f, XZOffset.Y);

	// Set location
	SetWorldLocation(BoneWorldLocation + Offset3D);

	// Rotation
	float Pitch = RotationOffset;
	if (!FighterPawnRef->IsFacingRight())
	{
		Pitch = 180.f - Pitch;
	}

	SetWorldRotation(FRotator(Pitch, 0.f, 0.f));
}