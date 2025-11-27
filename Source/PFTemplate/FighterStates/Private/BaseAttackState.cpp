#include "BaseAttackState.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.h"
#include "FrameScriptRunner.h"

void UBaseAttackState::Tick(FFighterInput& NewInput)
{
	Super::Tick(NewInput);

	if (FighterPawnRef->FrameScriptRunner->IsFinished())
	{
		FighterPawnRef->SetCurrentAnimation("Idle", 2);
		StateMachine->TryChangeState("Idle", NewInput);
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