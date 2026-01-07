#include "JumpFromLedgeState.h"
#include "FighterPawn.h"

bool UJumpFromLedgeState::HandleTimer(int32 FramesInState)
{
	if (FighterPawnRef->AnimFinished())
	{
		StateMachine->ChangeFighterState("JumpUp");
		return true;
	}
	return false;
}
