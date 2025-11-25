#include "IdleState.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.h"

void UIdleState::OnExit()
{
	bOnLedge = false;
}

bool UIdleState::HandlePhysics(FFighterInput &NewInput)
{

	MoveComp->ApplyGroundFriction();
	MoveComp->PreventLedgeFall(false);
	
	if (MoveComp->IsAirborne())
	{
		FighterPawnRef->SetCurrentAction("Falling");
		FighterPawnRef->StateMachine->TryChangeState("Falling", NewInput);
		return true;
	}

	if (MoveComp->IsStandingOnFacingLedge() && (MoveComp->GetVelocity().X == FIXED_32(0.f)) && !bOnLedge)
	{
		FighterPawnRef->SetCurrentAction("Teeter");
		bOnLedge = true;
	}

	return false;
}

bool UIdleState::HandleButtonInput(FFighterInput &NewInput)
{
	FButtonState &ButtonState = NewInput.Button;
	
	if (ButtonState.IsPressed(EInputButton::Jump))
	{
		StateMachine->TryChangeState("JumpSquat", NewInput);
		return true;
	}

	if (ButtonState.IsPressed(EInputButton::Shield) || ButtonState.IsHeld(EInputButton::Shield))
	{
		StateMachine->TryChangeState("Shield", NewInput);
		return true;
	}

	if (ButtonState.IsPressed(EInputButton::Attack))
	{
		StateMachine->TryChangeState("GroundAttack", NewInput); //possibly need to update
		return true;
	}

	return false;
}

bool UIdleState::HandleStickInput(FFighterInput& NewInput)
{
	FStickState &StickState = NewInput.Stick;
	if (StickState.bFlick)
	{
		FighterPawnRef->StateMachine->TryChangeState("Dash", NewInput);
		return true;
	}
	
	if (StickState.bWalking)
	{
		StateMachine->TryChangeState("Walking", NewInput);
		return true;
	}

	return false;
}