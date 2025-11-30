#include "Hurtbox2D.h"
#include "GameFramework/Actor.h"
#include "DrawDebugHelpers.h"
#include "FighterPawn.h"
#include "Components/CapsuleComponent.h"

AHurtbox2D::AHurtbox2D()
{
	bIsActive = true;
	PrimaryActorTick.bCanEverTick = false;
	SetActorTickEnabled(false);
}

void AHurtbox2D::Initialize(AFighterPawn* InOwner)
{
	Super::Initialize(InOwner);
	SetInvincibility(false);
}

void AHurtbox2D::TickHurtbox()
{
	UpdateRotation();
	UpdateLocation();
}

void AHurtbox2D::UpdateLocation()
{
	if (!FighterPawnRef || BoneName.IsNone())
		return;
	
	SetActorLocation(Fixed2DToVector(FighterPawnRef->GetBoneLocation(BoneName)));
}

void AHurtbox2D::UpdateRotation() //need to figure this one out
{
	if (!FighterPawnRef || BoneName.IsNone()) return;
	
	const FVector BoneLoc = Fixed2DToVector(FighterPawnRef->GetBoneLocation(BoneName));
	const FVector VecDir = FighterPawnRef->GetBoneVector(BoneName);
	const FVector AddedVectors = BoneLoc - VecDir;
	const float Dist = FVector::Dist({BoneLoc.X, 0.f, BoneLoc.Z}, {AddedVectors.X, 0.f, AddedVectors.Z});

	float FinalRot = 0.f;
	if (Dist > 0.1f)
	{
		const float AngleRadians = FMath::Atan2(
			AddedVectors.Z - BoneLoc.Z,
			AddedVectors.X - BoneLoc.X
		);
		FinalRot = FMath::RadiansToDegrees(AngleRadians);
	}
	
	SetActorRotation({FinalRot, 0.f, 0.f}, ETeleportType::None);
}

void AHurtbox2D::SetInvincibility(bool bInvulnerable)
{
	bIsInvincible = bInvulnerable;

	const FHitboxMaterialSet& MaterialSet = bIsInvincible ? InvulnerableColors : ActiveColors;

	if (MaterialSet.FillMaterial)
		CapsuleMesh->SetMaterial(0, MaterialSet.FillMaterial);

	if (MaterialSet.OutlineMaterial)
		CapsuleMesh->SetMaterial(1, MaterialSet.OutlineMaterial);
}