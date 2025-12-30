#include "ProjectileBase.h"
#include "FighterPawn.h"

void AProjectileBase::SetBoxActive(bool bActivate, const FHitboxDefinition& InDefinition)
{
	Super::SetBoxActive(bActivate, InDefinition);
	Velocity = Vector2DToFixed2D(InitialVelocity);
}

void AProjectileBase::StepFrame(int32 BattleFrame)
{
	FVector DesiredMove = Fixed2DToVector(Velocity * FixedDt);
	AddActorWorldOffset(DesiredMove, false);
}