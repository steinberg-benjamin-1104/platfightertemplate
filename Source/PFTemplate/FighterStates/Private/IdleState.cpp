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
		FighterPawnRef->StateMachine->ChangeFighterState("Falling", NewInput);
		return true;
	}

	if (MoveComp->IsStandingOnFacingLedge() && (MoveComp->GetVelocity().X == FFixed_32(0.f)) && !bOnLedge)
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
		StateMachine->ChangeFighterState("JumpSquat", NewInput);
		return true;
	}

	if (ButtonState.IsPressed(EInputButton::Shield) || ButtonState.IsHeld(EInputButton::Shield))
	{
		StateMachine->ChangeFighterState("Shield", NewInput);
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
	FStickState Stick = NewInput.Stick;
	EStickDir Stickdir = GetStickDirection(Stick.StickPos, FighterPawnRef->IsFacingRight());
	
	bool X = (Stickdir == EStickDir::Backward || Stickdir == EStickDir::Forward);
	if (Stick.bFlick && X)
	{
		NewInput.Stick.ConsumeFlick();
		FighterPawnRef->StateMachine->ChangeFighterState("Dash", NewInput);
		return true;
	}
	
	if (X)
	{
		StateMachine->ChangeFighterState("Walking", NewInput);
		return true;
	}

	return false;
}