#pragma once

#include "CoreMinimal.h"
#include "SafeMathBP.h"

#include "HitboxData.generated.h"

UENUM(BlueprintType)
enum class EHitEffect : uint8
{
	None       UMETA(DisplayName = "None"),
	Knockback  UMETA(DisplayName = "Knockback"),
	Clash      UMETA(DisplayName = "Clash"),
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

	bool HasSamePriority(const FDamageInfo& Other) const { return Priority == Other.Priority; }
	bool HasHigherPriority(const FDamageInfo& Other) const { return Priority > Other.Priority; }
};

USTRUCT(BlueprintType)
struct FHitboxTransform
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName BoneName = "root";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVec2DBP Size;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FFixed_32BP Rotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVec2DBP LocationFromBone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EHitboxType HitboxType = EHitboxType::Normal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsAttached = false;

	FHitboxTransform()
	{
		Size.X = 20.f;
		Size.Z = 20.f;
		Rotation.Value = 0.f;
	}
};

USTRUCT(BlueprintType)
struct FHitboxDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FHitboxTransform Transform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDamageInfo DamageInfo;
};

USTRUCT(BlueprintType)
struct FHitboxGroup
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FHitboxDefinition> Hitboxes;
};