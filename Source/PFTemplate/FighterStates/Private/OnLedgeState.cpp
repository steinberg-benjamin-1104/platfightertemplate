#include "OnLedgeState.h"
#include "FighterMovementComponent.h"
#include "FighterPawn.h"

void UOnLedgeState::OnEnter(FFighterInput& Input)
{
	MoveComp->StopMovementCompletely(true);
	FighterPawnRef->SetCurrentAnimation("Ledgegrab");
	MoveComp->ResetJumpCount();
	Actionable = false;
}

bool UOnLedgeState::HandleTimer(FFighterInput& Input, int32 FramesInState)
{
	if (StateMachine->FramesInState == DelayAction) Actionable = true;
	return false;
}

bool UOnLedgeState::HandleButtonInput(FFighterInput& Input)
{
	if (!Actionable) return false;
	FButtonState ButtonState = Input.Button;
	
	if (ButtonState.IsPressed(EInputButton::Jump))
	{
		FighterPawnRef->SetFixedLoc(FighterPawnRef->GetFixedLoc() + FFixedVector2D(0.f, 200.f));
		MoveComp->DoHop(EHopType::Full);
		StateMachine->TryChangeState("Rising", Input);
		return true;
	}
	return false;
}

void UOnLedgeState::OnExit()
{
	MoveComp->bDoCollisionChecks = true;
}