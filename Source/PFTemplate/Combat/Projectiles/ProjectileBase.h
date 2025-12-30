#pragma once

#include "CoreMinimal.h"
#include "Hitbox2D.h"
#include "ProjectileBase.generated.h"


UCLASS()
class PFTEMPLATE_API AProjectileBase : public AHitbox2D
{
	GENERATED_BODY()

public:
	void StepFrame(int32 BattleFrame);
	virtual void SetBoxActive(bool bActivate, const FHitboxDefinition& InDefinition) override;

protected:

	virtual void UpdateLocation() override;
	virtual void UpdateRotation() override;

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

private:
	
	FFixedVector2D Velocity;
};