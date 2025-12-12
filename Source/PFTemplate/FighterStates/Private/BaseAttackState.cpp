#include "BaseAttackState.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.h"
#include "FrameScriptRunner.h"

void UBaseAttackState::Tick(FFighterInput& NewInput, int32 FramesInState)
{
	Super::Tick(NewInput, FramesInState);

	if (FighterPawnRef->FrameScriptRunner->IsFinished())
	{
		FighterPawnRef->SetCurrentAnimation("Idle", 2);
		StateMachine->ChangeFighterState("Idle", NewInput);
	}
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