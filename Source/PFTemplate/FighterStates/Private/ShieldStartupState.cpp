#include "ShieldStartupState.h"
#include "FighterPawn.h"

void UShieldStartupState::OnEnter()
{
	FighterPawnRef->SetCurrentAnimation("ShieldStartup");
}

bool UShieldStartupState::HandleTimer(int32 FramesInState)
{
	if (FramesInState == Duration)
	{
		StateMachine->ChangeFighterState("Shield");
		return true;
	}
	return false;
}

bool UShieldStartupState::HandlePhysics()
{
	MoveComp->ApplyGroundFriction();
	MoveComp->PreventLedgeFall(false);
	return false;
}

void UShieldStartupState::HandleInput()
{
	EStickDir StickDir = GetCurrentStickDir();

	if (StickDir == EStickDir::Forward && InputBuffer->WasPressed(EInputButton::Flick))
	{
		FighterPawnRef->FlipFacingDirection();
		if (FighterPawnRef->SetCurrentAnimation("ForwardRoll"))
			StateMachine->ChangeFighterState("MiscAnim");
		return;
	}

	if (StickDir == EStickDir::Backward && InputBuffer->WasPressed(EInputButton::Flick))
	{
		if (FighterPawnRef->SetCurrentAnimation("BackRoll"))
			StateMachine->ChangeFighterState("MiscAnim");
		return;
	}
}