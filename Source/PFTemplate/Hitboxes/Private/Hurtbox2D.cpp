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

	FVector boneloc = FighterPawnRef->GetBakedSocketLocation(BoneName);
	boneloc.X = boneloc.Y;
	FVector loc = boneloc + FighterPawnRef->GetActorLocation();
	
	SetActorLocation(FVector(loc.X, 0.f, loc.Z));
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