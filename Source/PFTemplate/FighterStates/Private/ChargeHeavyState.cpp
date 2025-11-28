#include "ChargeHeavyState.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.h"

void UChargeHeavyState::OnEnter(FFighterInput& Input)
{
	FighterPawnRef->bStopAnimUpdates = true;
}

bool UChargeHeavyState::HandleTimer(FFighterInput& Input, int32 FramesInState)
{
	if (StateMachine->FramesInState == MaxHoldDuration)
	{
		StateMachine->TryChangeState("BaseAttack", Input); //possibly needs a change here
		return true;
	}
	return false;
}

bool UChargeHeavyState::HandleButtonInput(FFighterInput& Input)
{
	FButtonState ButtonState = Input.Button;

	if (!ButtonState.IsPressed(EInputButton::Attack))
	{
		StateMachine->TryChangeState("BaseAttack", Input); //possibly needs a change here
		return true;
	}
	return false;
}

bool UChargeHeavyState::HandlePhysics(FFighterInput& Input)
{
	MoveComp->ApplyGroundFriction();
	MoveComp->PreventLedgeFall(true);
	return false;
}


void UChargeHeavyState::OnExit()
{
	FighterPawnRef->bStopAnimUpdates = false;
}