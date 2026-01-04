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

void UProjectilePool::PreCollision()
{
	for (AProjectileBase* Proj : ActivePool)
	{
		if (Proj->PreCollision()) ActivePool.Remove(Proj);
	}
}

void UProjectilePool::DetectCollision()
{
	for (AProjectileBase* Proj : ActivePool) Proj->DetectCollisions();
}

void UProjectilePool::ProcessHits()
{
	for (AProjectileBase* Proj : ActivePool)
		if (Proj->ProcessHits()) ActivePool.Remove(Proj);
}