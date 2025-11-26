#pragma once
#include "FrameCommand.h"
#include "Engine/DataTable.h"
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
	TArray<FFrameCommand> Commands;
};