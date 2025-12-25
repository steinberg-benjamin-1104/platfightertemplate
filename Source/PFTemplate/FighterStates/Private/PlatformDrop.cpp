#include "PlatformDrop.h"
#include "FighterPawn.h"

void UPlatformDropState::OnEnter(FFighterInput& Input)
{
	FighterPawnRef->SetCurrentAnimation("Falling"); //change to platform drop
	MoveComp->bIgnorePlatform = true;
	MoveComp->bOnPlatform = false;
	MoveComp->SetMovementMode(EFighterMovementMode::Falling);
}

void UPlatformDropState::OnExit()
{
	MoveComp->bIgnorePlatform = false;
}

bool UPlatformDropState::HandleTimer(FFighterInput& Input, int32 FramesInState)
{
	if (FramesInState == Duration)
	{
		StateMachine->ChangeFighterState("Falling", Input);
		return true;
	}
	return false;
}
