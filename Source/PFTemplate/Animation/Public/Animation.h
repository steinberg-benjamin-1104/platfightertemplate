#pragma once
#include "FrameCommand.h"
#include "Engine/DataTable.h"
#include "AnimMvmt.h"
#include "BakedSockets.h"
#include "Animation.generated.h"

USTRUCT(BlueprintType)
struct FAnimation : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBakedAnimation* BakedAnimation = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsLoop = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMoveLogicAsset* CommandScript;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBakedAnimMvmt* BakedAnimMvmt = nullptr;
};