#pragma once

#include "FighterMovementMode.h"
#include "SafeMathBP.h"
#include "AnimMvmt.generated.h"

UENUM(BlueprintType)
enum class EPostMovementVelocityMode : uint8
{
	Keep,
	Zero,
	Override
};

USTRUCT(BlueprintType)
struct FAxisMovement
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* Curve = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FFixed_32BP TotalDisplacement;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPostMovementVelocityMode PostMovement = EPostMovementVelocityMode::Keep;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FFixed_32BP OverrideVelocity;
};

USTRUCT(BlueprintType)
struct FAnimMvmt
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 StartFrame = 0; //start of motion

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 EndFrame = 0; //last frame of motion
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAxisMovement X;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAxisMovement Z;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bPreventLedgeFall = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EFighterMovementMode Mode = EFighterMovementMode::None;
};