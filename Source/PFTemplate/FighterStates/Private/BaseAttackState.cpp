#include "BaseAttackState.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.h"
#include "FrameScriptRunner.h"

bool UBaseAttackState::HandleTimer(FFighterInput &NewInput, int32 FramesInState)
{
	if (FighterPawnRef->AnimFinished())
	{
		FighterPawnRef->SetCurrentAnimation("Idle", 2);
		StateMachine->ChangeFighterState("Idle", NewInput);
		return true;
	}
	return false;
}

bool UBaseAttackState::HandlePhysics(FFighterInput& NewInput)
{
	MoveComp->ApplyGroundFriction();
	MoveComp->PreventLedgeFall(true);
	return false;
}

void UBaseAttackState::OnExit()
{
	FighterPawnRef->GetHitboxManager()->DeactivateHitboxes(true);
	FighterPawnRef->MovementComponent->bCanApplyGroundFriction = true;
}