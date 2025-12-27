#include "JumpUpState.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.h"

void UJumpUpState::OnEnter()
{
	FighterPawnRef->SetCurrentAnimation("JumpUp");
}

bool UJumpUpState::HandleTimer(int32 FramesInState)
{
	if (FramesInState == Duration) MoveComp->StartGroundJump();
	return false;
}

bool UJumpUpState::HandlePhysics()
{
	if (MoveComp->GetCurrentMode() == EFighterMovementMode::Falling)
	{
		StateMachine->ChangeFighterState("Falling");
		return true;
	}
	
	MoveComp->ApplyAirDrift(InputBuffer->GetRecent().StickPos.X);
	return false;
}