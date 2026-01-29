#pragma once

#include "CoreMinimal.h"
#include "BakedAnimation.h"
#include "AnimCursor.generated.h"

USTRUCT()
struct FAnimCursor
{
	GENERATED_BODY()

	FAnimCursor(UBakedAnimation* InAnimation, bool InLoop)
	{
		Animation = InAnimation;
		bLooping = InLoop;
	}
	
	UPROPERTY()
	const UBakedAnimation* Animation = nullptr;

	UPROPERTY()
	int32 Frame = -1;

	UPROPERTY()
	bool bLooping = false;

	bool IsValid() const { return Animation != nullptr; }

	int32 GetTotalFrames() const
	{
		return Animation ? Animation->GetNumFrames() : 0;
	}

	void Advance()
	{
		if (!Animation) return;

		const int32 Total = GetTotalFrames();
		if (bLooping) Frame = (Frame + 1) % Total;
		else Frame = FMath::Min(Frame + 1, Total);
	}
};