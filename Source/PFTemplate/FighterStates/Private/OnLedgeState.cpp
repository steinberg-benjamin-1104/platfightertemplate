#include "OnLedgeState.h"
#include "FighterMovementComponent.h"
#include "FighterPawn.h"

void UOnLedgeState::OnEnter()
{
	MoveComp->StopMovementCompletely(true);
	FighterPawnRef->SetCurrentAnimation("Ledgegrab");
	MoveComp->ResetJumpCount();
	Actionable = false;
}

bool UOnLedgeState::HandleTimer(int32 FramesInState)
{
	if (StateMachine->FramesInState == DelayAction) Actionable = true;
	return false;
}

void UOnLedgeState::HandleInput()
{
	if (!Actionable) return;
	
	if (FFighterInput* Input = InputBuffer->WasPressed(EInputButton::Jump))
	{
		FighterPawnRef->SetFixedLoc(FighterPawnRef->GetFixedLoc() + FFixedVector2D(0.f, 200.f));
		MoveComp->StartJump(EJumpType::Full);
		Input->Consume(EInputButton::Jump);
		StateMachine->ChangeFighterState("JumpUp");
	}
}

void UOnLedgeState::OnExit()
{
	MoveComp->bDoCollisionChecks = true;
}