#include "ObjectPool.h"
#include "Poolable.h"

void UObjectPool::Initialize(AFighterPawn* InOwner, int32 PoolSize)
{
	FighterPawnRef = InOwner;

	for (int32 i = 0; i < PoolSize; ++i)
	{
		AActor* NewActor = GetWorld()->SpawnActor<AActor>(PoolClass);

		if (NewActor)
		{
			if (IPoolable* Poolable = Cast<IPoolable>(NewActor))
			{
				Poolable->Initialize(InOwner);
				Pool.Add(NewActor);
			}
		}
	}
}

AActor* UObjectPool::RequestObjectFromPool()
{
	for (AActor* Actor : Pool)
	{
		if (Actor)
		{
			if (IPoolable* Poolable = Cast<IPoolable>(Actor))
			{
				if (!Poolable->IsActive())
					return Actor;
			}
		}
	}

	UE_LOG(LogTemp, Fatal, TEXT("Projectile pool exhausted!"));
	return nullptr;
}