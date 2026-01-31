#pragma once
#include "CoreMinimal.h"

#include "Windows.generated.h"

class UAttackDataAsset;

UENUM(BlueprintType)
enum class EAttackWindowType : uint8
{
	CancelOnWhiff,
	CancelOnHit,
	Movement,
	HitboxGroup
};

USTRUCT(BlueprintType)
struct FAttackMovement
{
	GENERATED_BODY()

	FVector2D Velocity;
	FVector2D Acceleration = FVector2D(1.f, 1.f);
	bool bKeepVelocity = true;
};

USTRUCT(BlueprintType)
struct FAttackWindow
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	EAttackWindowType Type;

	UPROPERTY(EditAnywhere)
	int32 StartFrame = 0;

	UPROPERTY(EditAnywhere)
	int32 EndFrame = 0;

	// Hitbox
	UPROPERTY(EditAnywhere, meta=(EditCondition="Type == EAttackWindowType::Hitbox", EditConditionHides))
	TArray<int32> HitboxGroupIndices;

	// Whiff
	UPROPERTY(EditAnywhere, meta=(EditCondition="Type == EAttackWindowType::CancelOnWhiff", EditConditionHides))
	TArray<TObjectPtr<UAttackDataAsset>> CancelWhiffTargets;

	// Hit
	UPROPERTY(EditAnywhere, meta=(EditCondition="Type == EAttackWindowType::CancelOnHit", EditConditionHides))
	TArray<TObjectPtr<UAttackDataAsset>> CancelHitTargets;
	
	// Movement
	UPROPERTY(EditAnywhere, meta=(EditCondition="Type == EAttackWindowType::Movement", EditConditionHides))
	TArray<FAttackMovement> Movement;

	// Toggle Hurtbox Intangibility
	UPROPERTY(EditAnywhere, meta=(EditCondition="Type == EAttackWindowType::HurtboxIntangible", EditConditionHides))
	TArray<FName> Hurtboxes;
};