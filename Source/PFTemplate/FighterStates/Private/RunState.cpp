#include "RunState.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.h"
#include "StickDirection.h"

void URunState::OnEnter(FFighterInput& NewInput)
{
	FighterPawnRef->SetCurrentAnimation("Run");
	FFixedVector2D Velocity = MoveComp->GetVelocity();
	Velocity.X = FighterPawnRef->GetFacingDirection() * MoveComp->RunSpeed.ToFixed();
	MoveComp->SetVelocity(Velocity);
}

bool URunState::HandleStickInput(FFighterInput& Input)
{
	EStickDir StickDir = GetStickDirection(Input.Stick.StickPos, FighterPawnRef->IsFacingRight());

	if (StickDir == EStickDir::Backward)
	{
		StateMachine->ChangeFighterState("Skid", Input);
		return true;
	}
	
	if (StickDir == EStickDir::Center)
	{
		FighterPawnRef->SetCurrentAnimation("Idle", 3);
		StateMachine->ChangeFighterState("Idle", Input);
		MoveComp->HaltHorizontalVelocity();
		return true;
	}
	return false;
}

bool URunState::HandleButtonInput(FFighterInput& NewInput)
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

	return false;
}

bool URunState::HandlePhysics(FFighterInput& Input)
{
	MoveComp->PreventLedgeFall(false);
	
	if (MoveComp->GetCurrentMode() == EFighterMovementMode::Falling)
	{
		FighterPawnRef->SetCurrentAnimation("Falling");
		FighterPawnRef->StateMachine->ChangeFighterState("Falling", Input);
		return true;
	}
	return false;
}