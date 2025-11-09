#pragma once

#include "CoreMinimal.h"
#include "AnimPlaybackData.generated.h"

USTRUCT(BlueprintType)
struct FAnimPlaybackData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AnimControl")
	UAnimSequenceBase* Sequence = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AnimControl")
	int32 Frame = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AnimControl")
	int32 TotalFrames = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AnimControl")
	bool bLooping = false;

	FAnimPlaybackData(UAnimSequenceBase* InSequence = nullptr, int32 InFrame = 0, int32 InTotalFrames = 0, bool bInLooping = false)
	{
		Sequence = InSequence;
		Frame = InFrame;
		TotalFrames = InTotalFrames;
		bLooping = bInLooping;
	}

	void Reset()
	{
		Sequence = nullptr;
		Frame = 0;
		TotalFrames = 0;
		bLooping = false;
	}

	bool IsValid() const { return Sequence && TotalFrames > 0; }

	void AdvanceFrame()
	{
		if (!IsValid())
			return;

		if (bLooping)
		{
			Frame = (Frame + 1) % TotalFrames;
		}
		else
		{
			Frame = FMath::Min(Frame + 1, TotalFrames - 1);
		}
	}
};