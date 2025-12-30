#pragma once
#include "CoreMinimal.h"

#include "ProjectilePool.generated.h"

class AProjectileBase;

UCLASS()
class PFTEMPLATE_API AProjectilePool : public AActor
{
	GENERATED_BODY()

public:
	AProjectileBase* Request();
	void Return(AProjectileBase* Projectile);

private:
	TArray<AProjectileBase*> Pool;
};
