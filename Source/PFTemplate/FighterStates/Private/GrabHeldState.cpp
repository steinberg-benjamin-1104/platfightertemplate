#include "GrabHeldState.h"
#include "FighterPawn.h"

void UGrabHeldState::OnEnter()
{
	FighterPawnRef->SetCurrentAnimation("GrabHeld");
	FighterPawnRef->UpdateAnimation();
	MoveComp->StopMovementCompletely();
	FFixedVector2D InstLoc = FighterPawnRef->LastInstigator->GetFixedLoc();
	FFixed_32 AdjLoc = FFixed_32(80.f) * FighterPawnRef->LastInstigator->GetFacingDirection();
	InstLoc.X += AdjLoc;
	FighterPawnRef->SetFixedLoc(InstLoc);
}