// WalkingState.cpp
#include "WalkingState.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.h"
#include "StickDirection.h"

void UWalkingState::OnEnter(FFighterInput& Input)
{
	FighterPawnRef->SetCurrentAnimation("Walk");
	EStickDir StickDir = GetStickDirection(Input.Stick.Current, FighterPawnRef->IsFacingRight());

	if (StickDir == EStickDir::Backward) FighterPawnRef->FlipFacingDirection();

	FFixedVector2D Velocity;
	Velocity.X = MoveComp->WalkSpeed.ToFixed() * FighterPawnRef->GetFacingDirection();
	MoveComp->SetVelocity(Velocity);
}

bool UWalkingState::HandlePhysics(FFighterInput& Input)
{
	MoveComp->PreventLedgeFall(true);
	return false;
}

bool UWalkingState::HandleStickInput(FFighterInput& Input)
{
	EStickDir StickDir = GetStickDirection(Input.Stick.Current, FighterPawnRef->IsFacingRight());

	FStickState &StickState = Input.Stick;
	if (StickState.bFlick)
	{
		FighterPawnRef->StateMachine->ChangeFighterState("Dash", Input);
		return true;
	}
	
	if (StickDir == EStickDir::Center)
	{
		FighterPawnRef->SetCurrentAnimation("Idle", 3);
		StateMachine->ChangeFighterState("Idle", Input);
		return true;
	}

	if (StickDir == EStickDir::Backward)
	{
		StateMachine->ChangeFighterState("Walk", Input);
		return true;
	}

	return false;
}
