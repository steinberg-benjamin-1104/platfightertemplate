#pragma once

#include "CoreMinimal.h"
#include "Hitbox2D.h"
#include "ProjectileBase.generated.h"


class AFighterPawn;
class AHitbox2D;

UCLASS()
class PFTEMPLATE_API AProjectileBase : public AActor
{
	GENERATED_BODY()

public:
	void StepFrame();
	void ActivateProjectile(const FHitboxDefinition& InDefinition);
	void Initialize(AFighterPawn* InPawn);
	bool IsActive() { return bIsActive; }
	virtual void UpdateLocation();
	bool PreCollision();
	void DetectCollisions();
	bool ProcessHits();
	
protected:

	void DeactivateProjectile();
	AProjectileBase();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D InitialVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D Acceleration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 LifeFrame = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxLifeFrame = 60;
	
	bool bDestroyOnHit = true;

	UPROPERTY()
	UChildActorComponent* Hitbox = nullptr;

	UPROPERTY() AHitbox2D* HitboxActor = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Mesh = nullptr;


private:
	
	FFixedVector2D Velocity;

	UPROPERTY() AFighterPawn* FighterPawnRef = nullptr;

	TArray<TScriptInterface<IHittable>> HitObjects;

	UPROPERTY() TArray<FPendingHit> PendingHits;

	bool bIsActive = false;
};