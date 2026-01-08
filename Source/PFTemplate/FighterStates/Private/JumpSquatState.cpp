#include "JumpSquatState.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.h"

void UJumpSquatState::OnEnter()
{
	FighterPawnRef->SetCurrentAnimation("JumpSquat");
}

bool UJumpSquatState::HandleTimer(int32 FramesInState)
{
	if (FramesInState == Duration)
	{
		bool bJumpButtonHeld = InputBuffer->IsHeld(EInputButton::Jump);
		StateMachine->ChangeFighterState("JumpUp");
		MoveComp->CachedJumpType = bJumpButtonHeld ? EJumpType::Full : EJumpType::Short;
		return true;
	}
	return false;
}

bool UJumpSquatState::HandlePhysics()
{
	MoveComp->ApplyGroundFriction();
	MoveComp->PreventLedgeFall(true);
	return false;
}