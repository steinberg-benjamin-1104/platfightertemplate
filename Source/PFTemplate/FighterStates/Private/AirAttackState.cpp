#include "AirAttackState.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.h"
#include "FrameScriptRunner.h"

bool UAirAttackState::HandleTimer(FFighterInput& Input, int32 FramesInState)
{
	if (FighterPawnRef->AnimFinished())
	{
		FighterPawnRef->SetCurrentAnimation("Falling", 5);
		StateMachine->ChangeFighterState("Falling", Input);
		return true;
	}
	return false;
}


bool UAirAttackState::HandlePhysics(FFighterInput& Input)
{
	if (MoveComp->GetCurrentMode() == EFighterMovementMode::Grounded)
	{
		FighterPawnRef->SetCurrentAnimation("Landing");
		StateMachine->ChangeFighterState("Idle", Input);
		FighterPawnRef->GetHitboxManager()->DeactivateHitboxes(true);
		return true;
	}
	return false;
}

bool UAirAttackState::HandleStickInput(FFighterInput& Input)
{
	if (!MoveComp->bIsFastFalling && Input.Stick.bDownThisFrame)
	{
		MoveComp->bIsFastFalling = true;
	}
	
	MoveComp->ApplyAirDrift(Input.Stick.StickPos.X);
	return false;
}
