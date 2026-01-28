#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AnimPlaybackData.h"
#include "SafeMath.h"
#include "FighterAnimInstance.generated.h"

UCLASS()
class PFTEMPLATE_API UFighterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AnimControl")
	FAnimPlaybackData Current;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AnimControl|Blend")
	FAnimPlaybackData Previous;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AnimControl|Blend")
	int32 BlendFrameCounter = 0;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AnimControl|Blend")
	float BlendAlpha = 0.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AnimControl|Blend")
	int32 BlendTotalFrames = 0;
	
	void SetAnimationSequence(UAnimSequence* NewSequence, bool bLoop, int32 NumFrames, int32 InBlendFrames);
	void AdvanceFrame();

	int32 GetCurrentFrameIndex() const {return Current.CurrentFrame; }
	bool CurrentAnimFinished() const {return Current.AnimIsFinished(); }

	bool bStopAnimUpdates = false;
};