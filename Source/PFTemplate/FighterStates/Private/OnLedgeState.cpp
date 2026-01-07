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
		MoveComp->CachedJumpType = EJumpType::Full;
		Input->Consume(EInputButton::Jump);
		if (FighterPawnRef->SetCurrentAnimation("LedgeJump"))
		{
			StateMachine->ChangeFighterState("OffLedge");
			return;
		}
	}

	if (FFighterInput* Input = InputBuffer->WasPressed(EInputButton::Shield))
	{
		Input->Consume(EInputButton::Shield);
		if (FighterPawnRef->SetCurrentAnimation("LedgeRoll"))
		{
			StateMachine->ChangeFighterState("OffLedge");
			return;
		}
	}

	if (FFighterInput* Input = InputBuffer->WasPressed(EInputButton::Attack))
	{
		Input->Consume(EInputButton::Attack);
		if (FighterPawnRef->SetCurrentAnimation("LedgeAttack"))
		{
			StateMachine->ChangeFighterState("OffLedge");
			return;
		}
	}

	EStickDir Stickdir = GetCurrentStickDir();

	if (Stickdir == EStickDir::Forward)
	{
		if (FighterPawnRef->SetCurrentAnimation("LedgeGetUp"))
		{
			StateMachine->ChangeFighterState("OffLedge");
			return;
		}
	}

	if (Stickdir == EStickDir::Up)
	{
		if (FighterPawnRef->SetCurrentAnimation("LedgeJump"))
		{
			StateMachine->ChangeFighterState("OffLedge");
			return;
		}
	}
	
	if (Stickdir == EStickDir::Backward || Stickdir == EStickDir::Down)
	{
		if (InputBuffer->GetRecent()->IsPressed(EInputButton::StickDown))
		{
			InputBuffer->GetRecent()->Consume(EInputButton::StickDown);
		}
		StateMachine->ChangeFighterState("Falling");
		return;
	}
}

void UOnLedgeState::OnExit()
{
	MoveComp->bDoCollisionChecks = true;
}