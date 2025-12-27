#include "AirAttackState.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.h"
#include "FrameScriptRunner.h"

bool UAirAttackState::HandleTimer(int32 FramesInState)
{
	if (FighterPawnRef->AnimFinished())
	{
		FighterPawnRef->SetCurrentAnimation("Falling", 5);
		StateMachine->ChangeFighterState("Falling");
		return true;
	}
	return false;
}


bool UAirAttackState::HandlePhysics()
{
	if (MoveComp->GetCurrentMode() == EFighterMovementMode::Grounded)
	{
		FighterPawnRef->SetCurrentAnimation("Landing");
		StateMachine->ChangeFighterState("Idle");
		FighterPawnRef->GetHitboxManager()->DeactivateHitboxes(true);
		return true;
	}
	return false;
}

void UAirAttackState::HandleInput()
{
	if (!MoveComp->bIsFastFalling && InputBuffer->GetRecent().IsPressed(EInputButton::StickDown))
	{
		MoveComp->bIsFastFalling = true;
		InputBuffer->GetRecent().Consume(EInputButton::StickDown);
	}
	
	MoveComp->ApplyAirDrift(InputBuffer->GetRecent().StickPos.X);
}