#pragma once

#include "CoreMinimal.h"

#include "HitboxData.generated.h"

UENUM(BlueprintType)
enum class EHitEffect : uint8
{
	None       UMETA(DisplayName = "None"),
	Knockback  UMETA(DisplayName = "Knockback"),
	Clash      UMETA(DisplayName = "Clash"),
	Grab       UMETA(DisplayName = "Grab"),
};

UENUM(BlueprintType)
enum class EHitboxType : uint8
{
	Normal    UMETA(DisplayName = "Normal"),
	Sour       UMETA(DisplayName = "Sour"),
	Sweet      UMETA(DisplayName = "Sweet"),
};

USTRUCT(BlueprintType)
struct FDamageInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Damage = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EHitEffect HitEffect = EHitEffect::Knockback;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Angle = 45;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BaseKnockback = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 KnockbackGrowth = 80;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 HitstopMod = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Priority = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName OnGrabState;

	bool HasSamePriority(const FDamageInfo& Other) const { return Priority == Other.Priority; }
	bool HasHigherPriority(const FDamageInfo& Other) const { return Priority > Other.Priority; }
};

USTRUCT(BlueprintType)
struct FHitboxTransform
{
	GENERATED_BODY()

	//visual indication
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EHitboxType HitboxType = EHitboxType::Normal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName BoneName = "root";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D Size;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Rotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D LocationFromBone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsAttached = false;
};

USTRUCT(BlueprintType)
struct FHitboxGroup
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	FName GroupName;
	
	UPROPERTY(EditAnywhere)
	FDamageInfo Damage;
	
	UPROPERTY(EditAnywhere)
	TArray<FHitboxTransform> Hitboxes;
};