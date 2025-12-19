#include "JumpUpState.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.h"

void UJumpUpState::OnEnter(FFighterInput& Input)
{
	FighterPawnRef->SetCurrentAnimation("JumpUp");
}

bool UJumpUpState::HandlePhysics(FFighterInput& Input)
{
	MoveComp->ApplyAirDrift(Input.Stick.StickPos.X);
	return false;
}

bool UJumpUpState::HandleStickInput(FFighterInput& Input)
{
	if (MoveComp->GetCurrentMode() == EFighterMovementMode::Falling)
	{
		StateMachine->ChangeFighterState("Falling", Input);
		return true;
	}
	return false;
}