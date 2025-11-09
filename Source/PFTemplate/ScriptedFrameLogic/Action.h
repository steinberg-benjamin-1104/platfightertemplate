#pragma once
#include "FrameCommand.h"
#include "Engine/DataTable.h"
#include "HitboxData.h"
#include "Action.generated.h"

USTRUCT(BlueprintType)
struct FAction : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimSequenceBase* AnimSequence = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsLoop = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FHitboxGroup> HitboxCollection;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FFrameCommand> Commands;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FFrameCommand OnFinishCmd;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName TargetState = "None";
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName NextMoveName = "None";
};