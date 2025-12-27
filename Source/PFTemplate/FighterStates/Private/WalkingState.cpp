// WalkingState.cpp
#include "WalkingState.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.h"
#include "StickDirection.h"

void UWalkingState::OnEnter(FFighterInput& Input)
{
	FighterPawnRef->SetCurrentAnimation("Walk");
	EStickDir StickDir = GetStickDirection(Input.Stick.StickPos, FighterPawnRef->IsFacingRight());

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
	EStickDir StickDir = GetStickDirection(Input.Stick.StickPos, FighterPawnRef->IsFacingRight());
	
	if (StickDir == EStickDir::Center)
	{
		FighterPawnRef->SetCurrentAnimation("Idle", 3);
		StateMachine->ChangeFighterState("Idle", Input);
		MoveComp->HaltHorizontalVelocity();
		return true;
	}

	if (StickDir == EStickDir::Backward)
	{
		FighterPawnRef->FlipFacingDirection();
		FFixedVector2D Velocity = MoveComp->GetVelocity();
		Velocity.X = MoveComp->WalkSpeed.ToFixed() * FighterPawnRef->GetFacingDirection();;
		MoveComp->SetVelocity(Velocity);
	}

	return false;
}