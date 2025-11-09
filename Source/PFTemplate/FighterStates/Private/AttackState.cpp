#include "AttackState.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.h"
#include "FrameScriptRunner.h"

void UAttackState::Tick()
{
	Super::Tick();

	MoveComp->ApplyGroundFriction();
	MoveComp->HandleLedgeOrFall(true);

	if (FighterPawnRef->FrameScriptRunner->IsFinished())
	{
		FighterPawnRef->SetCurrentAction("Idle", 2);
		StateMachine->TryChangeState("Idle");
	}
}

void UAttackState::OnExit()
{
	Super::OnExit();
	FighterPawnRef->GetHitboxManager()->DeactivateHitboxes(true);
	FighterPawnRef->MovementComponent->bCanApplyGroundFriction = true;
}

bool UAttackState::Attack() //follow attack
{
	if (FighterPawnRef->FrameScriptRunner->AttackNotify())
	{
		FighterPawnRef->GetHitboxManager()->DeactivateHitboxes(true);
		FighterPawnRef->SetCurrentAction(FighterPawnRef->CurrentAction.NextMoveName);
		return true;
	}
	return false;
}