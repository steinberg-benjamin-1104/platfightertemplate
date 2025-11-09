#include "ShieldbreakState.h"
#include "Fighterpawn.h"

void UShieldbreakState::OnEnter()
{
	FighterPawnRef->ShieldComponent->ResetHealth();
	FighterPawnRef->SetCurrentAction("Shieldbreak");
}

void UShieldbreakState::Tick()
{
	if (StateMachine->FramesInState == Duration)
	{
		StateMachine->TryChangeState("Idle");
	}
}

void UShieldbreakState::OnExit()
{
	FighterPawnRef->SetCurrentAction("Idle");
}