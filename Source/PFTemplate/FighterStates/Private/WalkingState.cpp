// WalkingState.cpp
#include "WalkingState.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.h"
#include "StickDirection.h"

void UWalkingState::OnEnter()
{
	FighterPawnRef->SetCurrentAnimation("Walk");
	EStickDir StickDir = GetCurrentStickDir();

	if (StickDir == EStickDir::Backward) FighterPawnRef->FlipFacingDirection();

	FFixedVector2D Velocity;
	Velocity.X = MoveComp->WalkSpeed.ToFixed() * FighterPawnRef->GetFacingDirection();
	MoveComp->SetVelocity(Velocity);
}

bool UWalkingState::HandlePhysics()
{
	MoveComp->PreventLedgeFall(true);
	return false;
}

void UWalkingState::HandleInput()
{
	if (CheckActionButtons()) return;
	
	static const TMap<EInputButton, FName> ButtonToState = {
		{ EInputButton::Jump, "JumpSquat" },
		{ EInputButton::StickDown, "PlatformDrop" },
		{ EInputButton::Shield, "ShieldStartup"}
	};

	if (CheckBufferedButtonStateChanges(ButtonToState)) return;

	if (InputBuffer->IsHeld(EInputButton::Shield))
	{
		StateMachine->ChangeFighterState("ShieldStartup");
		return;
	}

	EStickDir Stickdir = GetCurrentStickDir();
	if (Stickdir == EStickDir::Backward || Stickdir == EStickDir::Forward)
	{
		if (InputBuffer->GetRecent()->IsPressed(EInputButton::Flick))
		{
			InputBuffer->GetRecent()->Consume(EInputButton::Flick);
			FighterPawnRef->StateMachine->ChangeFighterState("Dash");
			return;
		}
	}
	
	if (Stickdir == EStickDir::Center)
	{
		FighterPawnRef->SetCurrentAnimation("Idle", 3);
		StateMachine->ChangeFighterState("Idle");
		MoveComp->HaltHorizontalVelocity();
		return;
	}

	if (Stickdir == EStickDir::Backward)
	{
		FighterPawnRef->FlipFacingDirection();
		FFixedVector2D Velocity = MoveComp->GetVelocity();
		Velocity.X = MoveComp->WalkSpeed.ToFixed() * FighterPawnRef->GetFacingDirection();;
		MoveComp->SetVelocity(Velocity);
	}
}