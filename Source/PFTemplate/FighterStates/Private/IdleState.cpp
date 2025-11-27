#include "IdleState.h"
#include "FighterPawn.h"
#include "AttackDefinition.h"
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
		FighterPawnRef->SetCurrentAnimation("Falling");
		FighterPawnRef->StateMachine->TryChangeState("Falling", NewInput);
		return true;
	}

	if (MoveComp->IsStandingOnFacingLedge() && (MoveComp->GetVelocity().X == FIXED_32(0.f)) && !bOnLedge)
	{
		FighterPawnRef->SetCurrentAnimation("Teeter");
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
		return FighterPawnRef->TryStartAttack(EInputButton::Attack, NewInput);
	}

	if (ButtonState.IsPressed(EInputButton::Special))
	{
		return FighterPawnRef->TryStartAttack(EInputButton::Special, NewInput);
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