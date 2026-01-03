#include "ProjectilePool.h"
#include "ProjectileBase.h"
#include "FighterPawn.h"

bool UProjectilePool::ActivateProjectile()
{
	if (AProjectileBase* Proj = Cast<AProjectileBase>(RequestObjectFromPool()))
	{
		Proj->ActivateProjectile(FHitboxDefinition());
		ActivePool.Add(Proj);
		return true;
	}
	
	return false;
}

void UProjectilePool::FrameStep()
{
	for (int32 i = 0; i < ActivePool.Num(); i++)
	{
		AProjectileBase* Proj = Cast<AProjectileBase>(ActivePool[i]);
		Proj->StepFrame();
	}
}

void UProjectilePool::ProcessHits()
{
	for (int32 i = 0; i < ActivePool.Num(); i++)
	{
		AProjectileBase* Proj = Cast<AProjectileBase>(ActivePool[i]);
		if (Proj->ProcessHits()) ActivePool.Remove(Proj);
	}
}