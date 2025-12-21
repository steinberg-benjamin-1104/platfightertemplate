#include "JumpSquatState.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.h"

void UJumpSquatState::OnEnter(FFighterInput& Input)
{
	bJumpButtonHeld = true;
	FighterPawnRef->SetCurrentAnimation("JumpSquat");
}

bool UJumpSquatState::HandleTimer(FFighterInput& Input, int32 FramesInState)
{
	if (FramesInState == Duration)
	{
		StateMachine->ChangeFighterState("JumpUp", Input);
		return true;
	}
	return false;
}

bool UJumpSquatState::HandleButtonInput(FFighterInput& Input)
{
	FButtonState &ButtonState = Input.Button;
	
	if (!ButtonState.IsHeld(EInputButton::Jump))
	{
		if (!bJumpButtonHeld) return false;
		bJumpButtonHeld = false;
	}
	
	return false;
}

bool UJumpSquatState::HandlePhysics(FFighterInput& Input)
{
	MoveComp->ApplyGroundFriction();
	MoveComp->PreventLedgeFall(true);
	return false;
}

void UJumpSquatState::OnExit()
{
	MoveComp->DoHop(bJumpButtonHeld ? EHopType::Full : EHopType::Short);
}