#include "ShieldbreakState.h"
#include "Fighterpawn.h"

void UShieldbreakState::OnEnter()
{
	FighterPawnRef->ShieldComponent->ResetHealth();
	FighterPawnRef->SetCurrentAnimation("Shieldbreak");
}

bool UShieldbreakState::HandleTimer(int32 FramesInState)
{
	if (FramesInState == Duration)
	{
		StateMachine->ChangeFighterState("Idle");
		return true;
	}
	return false;
}

void UShieldbreakState::OnExit()
{
	FighterPawnRef->SetCurrentAnimation("Idle");
}