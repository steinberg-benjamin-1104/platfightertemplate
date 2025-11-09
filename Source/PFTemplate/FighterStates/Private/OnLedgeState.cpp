#include "OnLedgeState.h"
#include "FighterMovementComponent.h"
#include "FighterPawn.h"

void UOnLedgeState::OnEnter()
{
	MoveComp->StopMovementCompletely(true);
	FighterPawnRef->SetCurrentAction("Ledgegrab");
	MoveComp->ResetJumpCount();
	Actionable = false;
}

void UOnLedgeState::Tick()
{
	if (StateMachine->FramesInState == DelayAction) Actionable = true;
}

bool UOnLedgeState::Attack()
{
	return false;
}

bool UOnLedgeState::ShieldPressed()
{
	return false;
}

bool UOnLedgeState::JumpPressed()
{
	if (Actionable)
	{
		StateMachine->TryChangeState("Rising");
		FighterPawnRef->SetActorLocation(FighterPawnRef->GetActorLocation() + FVector(0, 0, 200));
		MoveComp->DoHop(EHopType::Full);
		return true;
	}
	return false;
}

void UOnLedgeState::OnExit()
{
	MoveComp->bDoCollisionChecks = true;
}