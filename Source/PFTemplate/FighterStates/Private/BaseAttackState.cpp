#include "BaseAttackState.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.h"
#include "FrameScriptRunner.h"

bool UBaseAttackState::HandleTimer(int32 FramesInState)
{
	if (FighterPawnRef->AnimFinished())
	{
		FighterPawnRef->SetCurrentAnimation("Idle", 2);
		StateMachine->ChangeFighterState("Idle");
		return true;
	}
	return false;
}

bool UBaseAttackState::HandlePhysics()
{
	MoveComp->ApplyGroundFriction();
	MoveComp->PreventLedgeFall(true);
	return false;
}

void UBaseAttackState::OnExit()
{
	FighterPawnRef->GetHitboxManager()->DeactivateHitboxes(true);
}