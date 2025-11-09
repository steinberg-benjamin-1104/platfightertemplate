#include "RisingState.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.h"

void URisingState::OnEnter()
{
	FighterPawnRef->SetCurrentAction("JumpUp");
}

void URisingState::Tick()
{
	UFighterState::Tick();

	// Transition to FallingState if movement mode changes
	if (MoveComp->GetCurrentMode() == EFighterMovementMode::Falling)
	{
		StateMachine->TryChangeState("Falling");
		return;
	}

	const float StickX = FighterPawnRef->GetPlayerStickInput().ToFloat().X;
	MoveComp->ApplyAirDrift(StickX);
	//MoveComp->CheckHorizontalCollision();
}