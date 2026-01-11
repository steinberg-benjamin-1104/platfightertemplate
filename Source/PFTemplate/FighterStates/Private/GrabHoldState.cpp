#include "GrabHoldState.h"
#include "FighterPawn.h"

void UGrabHoldState::OnEnter()
{
	FighterPawnRef->SetCurrentAnimation("GrabHold");
	FighterPawnRef->UpdateAnimation();
}

bool UGrabHoldState::HandlePhysics()
{
	MoveComp->ApplyGroundFriction();
	MoveComp->PreventLedgeFall(true);
	return false;
}