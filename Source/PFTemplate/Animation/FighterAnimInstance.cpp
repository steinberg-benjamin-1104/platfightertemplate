#include "FighterAnimInstance.h"

void UFighterAnimInstance::SetAnimationSequence(UAnimSequence* NewSequence, bool bLoop, int32 NumFrames, int32 InBlendFrames)
{
	FAnimPlaybackData NewAnim(NewSequence, -1, NumFrames, bLoop);
	Previous = Current;
	Current = NewAnim;
	Current.CurrentFrame = -1;
	BlendTotalFrames = InBlendFrames + 2;
	BlendAlpha = InBlendFrames == 0 ? 1.f : 0.f;
	BlendFrameCounter = 0;
}

int32 UFighterAnimInstance::AdvanceFrame()
{
	int32 Frame = Current.AdvanceFrame();
	
	if (BlendAlpha < 1.f && Current.IsValid() && Previous.IsValid())
	{
		BlendFrameCounter++;
		FFixed_32 TempAlpha = FixedAlphaFromFrame(BlendFrameCounter, BlendTotalFrames);
		BlendAlpha = FixedToFloat(TempAlpha);

		if (BlendFrameCounter >= BlendTotalFrames) BlendAlpha = 1.f;
	}
	else BlendAlpha = 1.f;
	return Frame;
}