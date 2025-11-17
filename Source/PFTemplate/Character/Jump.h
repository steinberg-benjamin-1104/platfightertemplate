#pragma once

#include "CoreMinimal.h"
#include "SafeMath.h"
#include "Jump.generated.h"

UENUM(BlueprintType)
enum class EHopType : uint8
{
	Full,
	Short,
	Air
};

USTRUCT(BlueprintType)
struct FHopData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIXED_32 Height = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Frames = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UCurveFloat> Curve = nullptr;
};