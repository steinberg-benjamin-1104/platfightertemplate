#include "DashState.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.h"
#include "StickDirection.h"
#include "AttackDefinition.h"

void UDashState::InitState(AFighterPawn* InFighterPawn, UFighterMovementComponent* InMoveComp, UFighterStateMachine* InStateMachine)
{
	Super::InitState(InFighterPawn, InMoveComp, InStateMachine);
	DashDuration = MoveComp->DashDuration;
}

void UDashState::OnEnter(FFighterInput& NewInput)
{
	FighterPawnRef->SetCurrentAnimation("Dash");

	EStickDir StickDir = GetStickDirection(NewInput.Stick.Current, FighterPawnRef->IsFacingRight());
	
	if (StickDir == EStickDir::Backward) FighterPawnRef->FlipFacingDirection();
	FFixedVector2D Velocity = MoveComp->GetVelocity();
	Velocity.X = FighterPawnRef->GetFacingDirection() * MoveComp->DashSpeed;
	MoveComp->SetVelocity(Velocity);
}

bool UDashState::HandleStickInput(FFighterInput& Input)
{
	EStickDir StickDir = GetStickDirection(Input.Stick.Current, FighterPawnRef->IsFacingRight());
	
	if (StickDir == EStickDir::Backward && Input.Stick.bFlick)
	{
		StateMachine->TryChangeState("Dash", Input);
		return true;
	}
	return false;
}

bool UDashState::HandlePhysics(FFighterInput& Input)
{
	MoveComp->PreventLedgeFall(false);

	if (MoveComp->IsAirborne())
	{
		FighterPawnRef->SetCurrentAnimation("Falling");
		FighterPawnRef->StateMachine->TryChangeState("Falling", Input);
		
		return true;
	}
	return false;
}

bool UDashState::HandleButtonInput(FFighterInput& NewInput)
{
	FButtonState &ButtonState = NewInput.Button;
	FStickState &StickState = NewInput.Stick;
	
	if (ButtonState.IsPressed(EInputButton::Jump))
	{
		StateMachine->TryChangeState("JumpSquat", NewInput);
		return true;
	}

	if (ButtonState.IsPressed(EInputButton::Shield) || ButtonState.IsHeld(EInputButton::Shield))
	{
		//stop x velocity here
		StateMachine->TryChangeState("Shield", NewInput);
		return true;
	}

	if (ButtonState.IsPressed(EInputButton::Attack))
	{
		//create attack function, this shit too long
	}

	return false;
}


bool UDashState::HandleTimer(FFighterInput& NewInput, int32 FramesInState)
{
	if (FramesInState == DashDuration)
	{
		EStickDir StickDir = GetStickDirection(NewInput.Stick.Current, FighterPawnRef->IsFacingRight());
		if (StickDir == EStickDir::Forward)
		{
			StateMachine->TryChangeState("Run", NewInput);
			return true;
		}
		if (StickDir == EStickDir::Backward && NewInput.Stick.bFlick)
		{
			StateMachine->TryChangeState("Dash", NewInput);
			return true;
		}
		
		FighterPawnRef->SetCurrentAnimation("Idle", 3);
		StateMachine->TryChangeState("Idle", NewInput);
	}

	return false;
}