#include "FighterAnimInstance.h"

void UFighterAnimInstance::SetAnimationSequence(UAnimSequenceBase* NewSequence, bool bLoop, int32 NumFrames, int32 InBlendFrames)
{
	FAnimPlaybackData NewAnim(NewSequence, -1, NumFrames, bLoop);
	Previous = Current;
	Current = NewAnim;
	Current.Frame = -1;
	BlendTotalFrames = InBlendFrames + 2;
	BlendAlpha = InBlendFrames == 0 ? 1.f : 0.f;
	BlendFrameCounter = 0;
}

void UFighterAnimInstance::AdvanceFrame()
{
	Current.AdvanceFrame();
	
	if (BlendAlpha < 1.f)
	{
		BlendFrameCounter++;
		FIXED_32 TempAlpha = FIXED_32(BlendFrameCounter) / FIXED_32(BlendTotalFrames);
		BlendAlpha = FixedToFloat(TempAlpha);

		if (BlendFrameCounter >= BlendTotalFrames) BlendAlpha = 1.f;
	}
}