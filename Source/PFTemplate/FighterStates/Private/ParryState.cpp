#include "ParryState.h"
#include "FighterPawn.h"

void UParryState::OnEnter()
{
	FighterPawnRef->SetCurrentAction("Parry");
	FighterPawnRef->bParry = true;
}

void UParryState::Tick()
{
	if (StateMachine->FramesInState == duration)
	{
		InEndLag = true;
		FighterPawnRef->SetCurrentAction("ParryRecovery");
	}

	if (InEndLag && StateMachine->FramesInState == duration + recovery)
	{
		FighterPawnRef->SetCurrentAction("Idle");
		StateMachine->TryChangeState("Idle");
	}
}

void UParryState::OnExit()
{
	FighterPawnRef->bParry = false;
	InEndLag = false;
}