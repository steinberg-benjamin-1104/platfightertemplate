#include "OffLedgeState.h"
#include "FighterMovementComponent.h"

void UOffLedgeState::OnEnter()
{
	MoveComp->bDoCollisionChecks = false;
}

void UOffLedgeState::OnExit()
{
	MoveComp->bDoCollisionChecks = true;
}