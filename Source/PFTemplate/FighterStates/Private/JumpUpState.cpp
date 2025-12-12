#include "JumpUpState.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.h"

bool UJumpUpState::HandlePhysics(FFighterInput& Input)
{
	MoveComp->ApplyAirDrift(Input.Stick.Current.X);
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