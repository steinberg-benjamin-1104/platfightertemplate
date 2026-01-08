#include "ShieldReleaseState.h"
#include "FighterPawn.h"

void UShieldReleaseState::OnEnter()
{
	FighterPawnRef->SetCurrentAnimation("ShieldRelease");
}

bool UShieldReleaseState::HandleTimer(int32 FramesInState)
{
	if (FramesInState >= duration)
	{
		FighterPawnRef->SetCurrentAnimation("Idle");
		StateMachine->ChangeFighterState("Idle");
	}
	return false;
}