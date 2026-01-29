#include "FighterAnimInstance.h"

void UFighterAnimInstance::SetAnimationSequence(UBakedAnimation* NewSequence, bool bLoop, int32 InBlendFrames)
{
	AnimSnapshot.SetNewAnim(FAnimCursor(NewSequence, bLoop), FBlendProfile(InBlendFrames));
}

void UFighterAnimInstance::AdvanceFrame()
{
	if (AnimSnapshot.bStopAnimUpdates) return;
	
	AnimSnapshot.Current.Advance();
	AnimSnapshot.Blend.Advance();
	
	UpdateAnimation(FixedToFloat(FixedDt), false);
}