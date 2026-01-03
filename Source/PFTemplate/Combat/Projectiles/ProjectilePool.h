#pragma once
#include "CoreMinimal.h"
#include "ObjectPool.h"
#include "ProjectilePool.generated.h"

class AProjectileBase;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PFTEMPLATE_API UProjectilePool : public UObjectPool
{
	GENERATED_BODY()

public:
	
	bool ActivateProjectile();
	virtual void FrameStep();
	void ProcessHits();
};