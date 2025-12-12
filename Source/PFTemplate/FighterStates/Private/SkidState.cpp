#include "SkidState.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.h"

void USkidState::OnEnter(FFighterInput& Input)
{
	FighterPawnRef->FlipFacingDirection();
	FighterPawnRef->SetCurrentAnimation("Skid");
	Reduction = MoveComp->GetVelocity().X.Abs() / SkidDuration;
}

bool USkidState::HandleTimer(FFighterInput& Input, int32 FramesInState)
{
	if (FramesInState == SkidDuration)
	{
		FighterPawnRef->SetCurrentAnimation("Idle");
		StateMachine->ChangeFighterState("Idle", Input);
		return true;
	}
	return false;
}

bool USkidState::HandlePhysics(FFighterInput& Input)
{
	FFixedVector2D Velocity = MoveComp->GetVelocity();
	Velocity.X -= Reduction * MoveComp->GetVelocity().X.Sign();
	MoveComp->SetVelocity(Velocity);
	MoveComp->PreventLedgeFall(true);
	return false;
}

bool USkidState::HandleButtonInput(FFighterInput& NewInput)
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

	return false;
}