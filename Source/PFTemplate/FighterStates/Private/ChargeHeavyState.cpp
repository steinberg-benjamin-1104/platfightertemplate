#include "ChargeHeavyState.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.h"

void UChargeHeavyState::OnEnter()
{
	FighterPawnRef->bStopAnimUpdates = true;
}

bool UChargeHeavyState::HandleTimer(int32 FramesInState)
{
	if (StateMachine->FramesInState == MaxHoldDuration)
	{
		StateMachine->ChangeFighterState("BaseAttack"); //possibly needs a change here
		return true;
	}
	return false;
}

void UChargeHeavyState::HandleInput()
{
	//handle attack release here
}

bool UChargeHeavyState::HandlePhysics()
{
	MoveComp->ApplyGroundFriction();
	MoveComp->PreventLedgeFall(true);
	return false;
}


void UChargeHeavyState::OnExit()
{
	FighterPawnRef->bStopAnimUpdates = false;
}