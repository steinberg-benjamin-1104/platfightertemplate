#include "Hurtbox2D.h"
#include "GameFramework/Actor.h"
#include "DrawDebugHelpers.h"
#include "FighterPawn.h"
#include "Components/CapsuleComponent.h"

AHurtbox2D::AHurtbox2D()
{
	PrimaryActorTick.bCanEverTick = false;
	SetActorTickEnabled(false);
}

void AHurtbox2D::Initialize(APawn* InPawn, FDeterministicCollisionWorld* CW)
{
	Super::Initialize(InPawn, CW);
	SetInvincibility(false);
	FighterPawnRef = Cast<AFighterPawn>(InPawn);
}

void AHurtbox2D::TickHurtbox()
{
	UpdateRotation();
	UpdateLocation();
}

void AHurtbox2D::UpdateLocation()
{
	if (!FighterPawnRef || BoneName.IsNone()) return;
	
	SetCapsuleLocation(FighterPawnRef->GetBakedSocketLocation(BoneName));
}

void AHurtbox2D::UpdateRotation()
{
	SetCapsuleRotation(FloatToFixed(FighterPawnRef->GetBakedBoneRotation(BoneName)));
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