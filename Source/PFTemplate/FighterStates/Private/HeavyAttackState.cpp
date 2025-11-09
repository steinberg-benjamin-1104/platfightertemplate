#include "HeavyAttackState.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.h"

void UHeavyAttackState::OnEnter()
{
	FighterPawnRef->bStopAnimUpdates = true;
	bEarlyRelease = !FighterPawnRef->FPC->IsAttackHeld();
}

void UHeavyAttackState::Tick()
{
	MoveComp->ApplyGroundFriction();
	MoveComp->HandleLedgeOrFall(true);

	if (StateMachine->FramesInState == MaxHoldDuration || bEarlyRelease)
	{
		StateMachine->TryChangeState("GroundAttack");
	}
}

void UHeavyAttackState::OnExit()
{
	Super::OnExit();
	bEarlyRelease = false;
	FighterPawnRef->bStopAnimUpdates = false;
	FighterPawnRef->FrameScriptRunner->ResetAttackNotify();
}

bool UHeavyAttackState::AttackRelease()
{
	StateMachine->TryChangeState("GroundAttack");
	return true;
}