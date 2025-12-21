#pragma once
#include "FrameCommand.h"
#include "Engine/DataTable.h"
#include "AnimMvmt.h"
#include "Animation.generated.h"

USTRUCT(BlueprintType)
struct FAnimation : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimSequenceBase* AnimSequence = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsLoop = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAnimMvmt AnimMvmt; //store curves and magnitue of 2d movement
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FFrameCommand> Commands; //stores frame number and cmds associated with frame
};