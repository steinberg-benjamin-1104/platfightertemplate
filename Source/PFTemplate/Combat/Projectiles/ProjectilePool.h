#pragma once
#include "CoreMinimal.h"
#include "ProjectilePool.generated.h"

class AProjectileBase;
class AFighterPawn;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PFTEMPLATE_API UProjectilePool : public UActorComponent
{
	GENERATED_BODY()

public:
	
	bool ActivateProjectile();
	
	virtual void Initialize(AFighterPawn* InOwner);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 PoolSize = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AProjectileBase> PoolClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D SpawnLocOffset;

	void PreCollision();
	void DetectCollision();
	void ProcessHits();

protected:

	UPROPERTY() TArray<AProjectileBase*> Pool;
	UPROPERTY() TArray<AProjectileBase*> ActivePool;
	
	AProjectileBase* RequestObjectFromPool();

	UPROPERTY() AFighterPawn* FighterPawnRef = nullptr;

	FVector DetermineSpawnLoc();
};