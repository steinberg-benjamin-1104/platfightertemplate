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

void AHurtbox2D::Initialize(APawn* InOwner)
{
	Super::Initialize(InOwner);
	SetInvincibility(false);
	FighterPawnRef = Cast<AFighterPawn>(InOwner);
}

void AHurtbox2D::TickHurtbox()
{
	UpdateRotation();
	UpdateLocation();
}

void AHurtbox2D::UpdateLocation()
{
	if (!FighterPawnRef || BoneName.IsNone()) return;

	FFixedVector2D Loc = FighterPawnRef->GetBakedSocketLocation(BoneName);
	SetActorLocation(Fixed2DToVector(Loc));
}

void AHurtbox2D::UpdateRotation()
{
	SetActorRotation({FighterPawnRef->GetBakedBoneRotation(BoneName), 0.f, 0.f}, ETeleportType::None);
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