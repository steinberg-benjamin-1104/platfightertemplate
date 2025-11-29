#include "AirAttackState.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.h"
#include "FrameScriptRunner.h"

bool UAirAttackState::HandleTimer(FFighterInput& Input, int32 FramesInState)
{
	if (FighterPawnRef->FrameScriptRunner->IsFinished())
	{
		FighterPawnRef->SetCurrentAnimation("Falling", 5);
		StateMachine->TryChangeState("Falling", Input);
		return true;
	}
	return false;
}


bool UAirAttackState::HandlePhysics(FFighterInput& Input)
{
	if (MoveComp->GetCurrentMode() == EFighterMovementMode::Grounded)
	{
		FighterPawnRef->SetCurrentAnimation("Landing");
		StateMachine->TryChangeState("Idle", Input);
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
	
	MoveComp->ApplyAirDrift(Input.Stick.Current.X);
	return false;
}
