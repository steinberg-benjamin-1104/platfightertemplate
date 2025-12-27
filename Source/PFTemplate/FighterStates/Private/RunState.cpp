#include "RunState.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.h"
#include "StickDirection.h"

void URunState::OnEnter()
{
	FighterPawnRef->SetCurrentAnimation("Run");
	FFixedVector2D Velocity = MoveComp->GetVelocity();
	Velocity.X = FighterPawnRef->GetFacingDirection() * MoveComp->RunSpeed.ToFixed();
	MoveComp->SetVelocity(Velocity);
}

void URunState::HandleInput()
{
	if (CheckActionButtons()) return;
	
	static const TMap<EInputButton, FName> ButtonToState = {
		{ EInputButton::Jump, "JumpSquat" },
		{ EInputButton::StickDown, "PlatformDrop" },
		{ EInputButton::Shield, "Shield"}
	};

	if (CheckBufferedButtonStateChanges(ButtonToState)) return;

	if (InputBuffer->IsHeld(EInputButton::Shield))
	{
		StateMachine->ChangeFighterState("Shield");
		return;
	}

	EStickDir StickDir = GetCurrentStickDir();

	if (StickDir == EStickDir::Backward)
	{
		StateMachine->ChangeFighterState("Skid");
		return;
	}
	
	if (StickDir == EStickDir::Center)
	{
		FighterPawnRef->SetCurrentAnimation("Idle", 3);
		StateMachine->ChangeFighterState("Idle");
		MoveComp->HaltHorizontalVelocity();
	}
	
}

bool URunState::HandlePhysics()
{
	MoveComp->PreventLedgeFall(false);
	
	if (MoveComp->GetCurrentMode() == EFighterMovementMode::Falling)
	{
		
		FighterPawnRef->StateMachine->ChangeFighterState("Falling");
		return true;
	}
	return false;
}