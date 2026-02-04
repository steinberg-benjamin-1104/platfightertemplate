#include "BaseAttackState.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.h"

bool UBaseAttackState::HandleTimer(int32 FramesInState)
{
	
}

bool UBaseAttackState::HandlePhysics()
{
	MoveComp->ApplyGroundFriction();
	MoveComp->PreventLedgeFall(true);
	return false;
}

void UBaseAttackState::OnExit()
{
	
}