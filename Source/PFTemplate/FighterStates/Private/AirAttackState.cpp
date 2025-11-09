#include "AirAttackState.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.h"
#include "FrameScriptRunner.h"

void UAirAttackState::Tick()
{
	// Transition to Falling when script finishes
	if (FighterPawnRef->FrameScriptRunner->IsFinished())
	{
		FighterPawnRef->SetCurrentAction("Falling", 5);
		StateMachine->TryChangeState("Falling");
		return;
	}
	
	// Check for landing (transition to Idle)
	if (MoveComp->GetCurrentMode() == EFighterMovementMode::Grounded)
	{
		FighterPawnRef->SetCurrentAction("Landing");
		StateMachine->TryChangeState("Idle");
		FighterPawnRef->GetHitboxManager()->DeactivateHitboxes(true);
		return;
	}

	// Fast-fall check
	if (!MoveComp->bIsFastFalling && FighterPawnRef->GetStickTracker()->WasDownJustPressedThisFrame() && MoveComp->GetCurrentMode() == EFighterMovementMode::Falling)
	{
		MoveComp->bIsFastFalling = true;
	}

	const float StickX = FighterPawnRef->GetPlayerStickInput().ToFloat().X;
	MoveComp->ApplyAirDrift(StickX);
}