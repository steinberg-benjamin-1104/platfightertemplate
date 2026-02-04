#include "FighterState.h"
#include "FighterPawn.h"

void UFighterState::InitState(AFighterPawn* InFighterPawn, UFighterMovementComponent* InMoveComp, UFighterStateMachine* InStateMachine, FInputBuffer* InInputBuffer)
{
	FighterPawnRef = InFighterPawn;
	MoveComp = InMoveComp;
	StateMachine = InStateMachine;
	InputBuffer = InInputBuffer;
}

EStickDir UFighterState::GetCurrentStickDir()
{
	return GetStickDirection(InputBuffer->GetRecent()->StickPos, FighterPawnRef->IsFacingRight());
}