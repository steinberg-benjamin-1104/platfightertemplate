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
		BlendAlpha = static_cast<float>(BlendFrameCounter) / static_cast<float>(BlendTotalFrames);

		if (BlendFrameCounter >= BlendTotalFrames) BlendAlpha = 1.f;
	}
}