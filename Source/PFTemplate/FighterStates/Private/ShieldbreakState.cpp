#include "ShieldbreakState.h"
#include "Fighterpawn.h"

void UShieldbreakState::OnEnter(FFighterInput& Input)
{
	FighterPawnRef->ShieldComponent->ResetHealth();
	FighterPawnRef->SetCurrentAnimation("Shieldbreak");
}

bool UShieldbreakState::HandleTimer(FFighterInput& Input, int32 FramesInState)
{
	if (FramesInState == Duration)
	{
		StateMachine->ChangeFighterState("Idle", Input);
		return true;
	}
	return false;
}

void UShieldbreakState::OnExit()
{
	FighterPawnRef->SetCurrentAnimation("Idle");
}