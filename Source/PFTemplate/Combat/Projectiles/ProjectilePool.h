#pragma once
#include "CoreMinimal.h"
#include "ProjectilePool.generated.h"

class AProjectileBase;
class AFighterPawn;
class AHitbox2D;

UCLASS(EditInlineNew, DefaultToInstanced, BlueprintType)
class PFTEMPLATE_API UProjectilePool : public UObject
{
	GENERATED_BODY()

public:
	
	bool ActivateProjectile();
	
	virtual void Initialize(AFighterPawn* InOwner);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 PoolSize = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AProjectileBase> PoolClass;

	void PreCollision();
	void DetectCollision();
	void ProcessHits();

protected:

	UPROPERTY() TArray<AProjectileBase*> Pool;
	UPROPERTY() TArray<AProjectileBase*> ActivePool;
	
	AProjectileBase* RequestObjectFromPool();

	UPROPERTY() AFighterPawn* FighterPawnRef = nullptr;
};