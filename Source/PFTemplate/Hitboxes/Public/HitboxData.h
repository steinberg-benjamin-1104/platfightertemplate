#pragma once

#include "CoreMinimal.h"

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
	int Angle = 45;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int BaseKnockback = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float KnockbackGrowth = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HitstopMod = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Priority = 2;

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
	FVector2D Size = FVector2D(10.f, 10.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Rotation = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D LocationFromBone = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EHitboxType HitboxType = EHitboxType::Normal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsAttached = false;
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