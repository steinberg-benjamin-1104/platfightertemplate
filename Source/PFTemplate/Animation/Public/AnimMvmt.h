#pragma once

#include "FighterMovementMode.h"
#include "AnimMvmt.generated.h"

USTRUCT(BlueprintType)
struct FAxisMovement
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* Curve = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TotalDisplacement;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool KeepVelocity = true;
};

UCLASS(BlueprintType)
class UBakedAnimMvmt : public UDataAsset
{
	GENERATED_BODY()
public:
	
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
	EFighterMovementMode TargetMode = EFighterMovementMode::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EFighterMovementMode FinishedMode = EFighterMovementMode::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector2D> DeltaPos;
};