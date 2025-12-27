#include "PlatformDrop.h"
#include "FighterPawn.h"

void UPlatformDropState::OnEnter()
{
	 //change to platform drop
	MoveComp->bIgnorePlatform = true;
	MoveComp->bOnPlatform = false;
	MoveComp->SetMovementMode(EFighterMovementMode::Falling);
}

void UPlatformDropState::OnExit()
{
	MoveComp->bIgnorePlatform = false;
}

bool UPlatformDropState::HandleTimer(int32 FramesInState)
{
	if (FramesInState == Duration)
	{
		StateMachine->ChangeFighterState("Falling");
		return true;
	}
	return false;
}
