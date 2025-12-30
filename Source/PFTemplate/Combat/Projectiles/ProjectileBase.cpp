#include "ProjectileBase.h"
#include "FighterPawn.h"

void AProjectileBase::Initialize(AFighterPawn* InOwner)
{
	FighterPawnRef = InOwner;
	HitboxManager->Initialize(FighterPawnRef, 2);
}

void AProjectileBase::SetActive(bool inActive, FName InName)
{
	bActive = inActive;
	if (bActive)
	{
		Velocity = Vector2DToFixed2D(InitialVelocity);
		HitboxManager->ActivateHitboxes(InName);
	}
}

void AProjectileBase::StepFrame(int32 BattleFrame)
{
	FVector DesiredMove = Fixed2DToVector(Velocity * FixedDt);
	AddActorWorldOffset(DesiredMove, false);
}