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
struct FJumpData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FFixed_32BP JumpHeight = 0.f;

	//peak height is reached at this frame
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 FramesToApex = 0;
};