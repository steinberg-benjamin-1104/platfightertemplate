#include "SpecialState.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.h"

bool USpecialState::HandleTimer(int32 FramesInState)
{
	if (FighterPawnRef->AnimFinished())
	{
		FighterPawnRef->SetCurrentAnimation("Falling", 2);
		StateMachine->ChangeFighterState("Falling");
		return true;
	}
	return false;
}

void USpecialState::OnEnter()
{
	
}

void USpecialState::OnExit()
{
	FighterPawnRef->GetHitboxManager()->DeactivateHitboxes(true);
	FighterPawnRef->MovementComponent->bCanApplyGroundFriction = true;
	MoveComp->CurrentMovementMode = EFighterMovementMode::Falling;
}