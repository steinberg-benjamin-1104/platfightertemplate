#include "JumpSquatState.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.h"

void UJumpSquatState::OnEnter()
{
	bJumpButtonHeld = true;
	FighterPawnRef->SetCurrentAnimation("JumpSquat");
}

bool UJumpSquatState::HandleTimer(int32 FramesInState)
{
	if (FramesInState == Duration)
	{
		StateMachine->ChangeFighterState("JumpUp");
		MoveComp->CachedJumpType = bJumpButtonHeld ? EJumpType::Full : EJumpType::Short;
		return true;
	}
	return false;
}

void UJumpSquatState::HandleInput()
{
	if (!InputBuffer->IsHeld(EInputButton::Jump) && bJumpButtonHeld)
	{
		bJumpButtonHeld = false;
	}
}

bool UJumpSquatState::HandlePhysics()
{
	MoveComp->ApplyGroundFriction();
	MoveComp->PreventLedgeFall(true);
	return false;
}