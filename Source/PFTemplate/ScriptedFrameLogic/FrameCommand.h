#pragma once

#include "CoreMinimal.h"
#include "EFrameCommandType.h"
#include "HitboxData.h"
#include "SafeMath.h"
#include "FrameCommand.generated.h"

USTRUCT(BlueprintType)
struct FFrameCommand
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EFrameCommandType Command = EFrameCommandType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FHitboxGroup HitboxGroup;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 FrameExecution = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool BoolParam = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 IntParam = 0;
	
	FFixedVector2D VectorParam = FFixedVector2D();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName NameParam = "none";
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UObject> Filepath;
};