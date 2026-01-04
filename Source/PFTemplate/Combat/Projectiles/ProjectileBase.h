#pragma once

#include "CoreMinimal.h"
#include "Hitbox2D.h"
#include "ProjectileBase.generated.h"


class AFighterPawn;
class AHitbox2D;

UCLASS(BlueprintType, Blueprintable)
class PFTEMPLATE_API AProjectileBase : public AActor
{
	GENERATED_BODY()

public:
	void ActivateProjectile(const FHitboxDefinition& InDefinition);
	void Initialize(AFighterPawn* InPawn);
	bool IsActive() { return bIsActive; }
	virtual void UpdateLocation();
	bool PreCollision();
	void DetectCollisions();
	bool ProcessHits();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D SpawnLocOffset;
	
protected:

	void DeactivateProjectile();
	AProjectileBase();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D InitialVelocity = FVector2D(200.f, 0.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D Acceleration = FVector2D(0.f, 0.f);;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 LifeFrame = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxLifeFrame = 60;
	
	bool bDestroyOnHit = true;

	UPROPERTY() AHitbox2D* HitboxActor = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Mesh = nullptr;


private:
	
	FFixedVector2D Velocity;

	UPROPERTY() AFighterPawn* FighterPawnRef = nullptr;

	TArray<TScriptInterface<IHittable>> HitObjects;

	UPROPERTY() TArray<FPendingHit> PendingHits;

	bool bIsActive = false;

	void SetSpawnLoc();
};