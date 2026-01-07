#include "MiscAnimState.h"
#include "FighterPawn.h"

bool UMiscAnimState::HandleTimer(int32 FramesInState)
{
	if (FighterPawnRef->AnimFinished())
	{
		FighterPawnRef->SetCurrentAnimation("Idle", 2);
		StateMachine->ChangeFighterState("Idle");
		return true;
	}
	return false;
}