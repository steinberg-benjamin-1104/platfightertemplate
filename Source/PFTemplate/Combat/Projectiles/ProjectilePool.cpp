#include "ProjectilePool.h"
#include "ProjectileBase.h"
#include "FighterPawn.h"

void UProjectilePool::Initialize(AFighterPawn* InOwner)
{
	FighterPawnRef = InOwner;

	for (int32 i = 0; i < PoolSize; ++i)
	{
		AProjectileBase* Proj = GetWorld()->SpawnActor<AProjectileBase>(PoolClass);

		if (Proj)
		{
			Proj->Initialize(InOwner);
			Pool.Add(Proj);
		}
	}
}

AProjectileBase* UProjectilePool::RequestObjectFromPool()
{
	for (AProjectileBase* Proj : Pool)
	{
		if (Proj && !Proj->IsActive()) return Proj;
	}

	UE_LOG(LogTemp, Fatal, TEXT("Projectile pool exhausted!"));
	return nullptr;
}

bool UProjectilePool::ActivateProjectile()
{
	if (AProjectileBase* Proj = RequestObjectFromPool())
	{
		Proj->ActivateProjectile(FHitboxDefinition());
		ActivePool.Add(Proj);
		return true;
	}
	
	return false;
}

void UProjectilePool::DetectCollision()
{
	for (AProjectileBase* Proj : ActivePool) Proj->DetectCollisions();
}

void UProjectilePool::PreCollision()
{
	for (int32 i = ActivePool.Num() - 1; i >= 0; --i)
	{
		if (ActivePool[i]->PreCollision())
		{
			ActivePool.RemoveAtSwap(i);
		}
	}
}

void UProjectilePool::ProcessHits()
{
	for (int32 i = ActivePool.Num() - 1; i >= 0; --i)
	{
		if (ActivePool[i]->ProcessHits())
		{
			ActivePool.RemoveAtSwap(i);
		}
	}
}