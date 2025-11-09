// WalkingState.cpp
#include "WalkingState.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.h"

void UWalkingState::OnEnter()
{
	FighterPawnRef->SetCurrentAction("Walk");
}

void UWalkingState::Tick()
{
	const FStickInt8 StickInput = FighterPawnRef->GetPlayerStickInput();
	const int8 StickX = StickInput.X;

	// Go idle if no movement input
	if (StickX == 0)
	{
		FighterPawnRef->SetCurrentAction("Idle", 3);
		StateMachine->TryChangeState("Idle");
		return;
	}

	// Determine direction (+1 or -1)
	const int8 StickDir = (StickX > 0) ? 1 : -1;
	FighterPawnRef->SetFacingDirection(StickDir);

	// Apply walking movement
	FVector Velocity = MoveComp->GetVelocity();
	Velocity.X = StickDir * MoveComp->WalkSpeed;
	MoveComp->SetVelocity(Velocity);
	MoveComp->HandleLedgeOrFall(true);
}