#pragma once

#include "CoreMinimal.h"
#include "SafeMath.h"
#include "ProjectileBase.generated.h"


UCLASS()
class PFTEMPLATE_API AProjectileBase : public AActor
{
	GENERATED_BODY()

public:
	void Initialize(AFighterPawn* InOwner);
	void SetActive(bool inActive, FName inName);
	void StepFrame(int32 BattleFrame);

protected:
	UPROPERTY()
	UHitboxManagerComponent* HitboxManager;

	UPROPERTY()
	AFighterPawn* FighterPawnRef = nullptr;

	// movement

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D InitialVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D Accel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 LifeFrame = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxLifeFrame = 60;

	// behavior flags
	bool bDestroyOnHit = true;
	bool bCanPierce = false;

	void OnHit(const FPendingHit& Hit);

private:
	bool bActive = false;
	FFixedVector2D Velocity;
};