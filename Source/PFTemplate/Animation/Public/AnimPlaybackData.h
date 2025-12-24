#pragma once

#include "CoreMinimal.h"
#include "AnimPlaybackData.generated.h"

USTRUCT(BlueprintType)
struct FAnimPlaybackData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AnimControl")
	UAnimSequence* Sequence = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AnimControl")
	int32 CurrentFrame = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AnimControl")
	int32 TotalFrames = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AnimControl")
	bool bLooping = false;

	FAnimPlaybackData(UAnimSequence* InSequence = nullptr, int32 InFrame = 0, int32 InTotalFrames = 0, bool bInLooping = false)
	{
		Sequence = InSequence;
		CurrentFrame = InFrame;
		TotalFrames = InTotalFrames;
		bLooping = bInLooping;
	}

	void Reset()
	{
		Sequence = nullptr;
		CurrentFrame = 0;
		TotalFrames = 0;
		bLooping = false;
	}

	bool IsValid() const { return Sequence && TotalFrames > 0; }

	int32 AdvanceFrame()
	{
		if (bLooping) CurrentFrame = (CurrentFrame + 1) % TotalFrames;
		else CurrentFrame += 1;
		return CurrentFrame;
	}

	bool AnimIsFinished() const { return CurrentFrame + 1 >= TotalFrames; } //temp fix
};