#include "ShieldState.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.h"

void UShieldState::InitState(AFighterPawn* InFighterPawn, UFighterMovementComponent* InMoveComp, UFighterStateMachine* InStateMachine)
{
	Super::InitState(InFighterPawn, InMoveComp, InStateMachine);
	ShieldComponent = InFighterPawn->ShieldComponent;
}

void UShieldState::OnEnter(FFighterInput& Input)
{
	FighterPawnRef->SetCurrentAnimation("Shield", 5);
}

bool UShieldState::HandleTimer(FFighterInput& Input, int32 FramesInState)
{
	if (FramesInState == StartupFrames)
	{
		ShieldComponent->SetShieldActive(true);
	}

	if (FramesInState >= FramesToEnd && InEndLag)
	{
		FighterPawnRef->SetCurrentAnimation("Idle");
		StateMachine->TryChangeState("Idle", Input);
		return true;
	}
	return false;
}

bool UShieldState::HandleButtonInput(FFighterInput& Input)
{
	FButtonState &ButtonState = Input.Button;
	
	if (ButtonState.IsPressed(EInputButton::Jump))
	{
		StateMachine->TryChangeState("JumpSquat", Input);
		return true;
	}

	if (!ButtonState.IsPressed(EInputButton::Shield))
	{
		if (!ShieldComponent->IsActive() || InEndLag) return false;
		FramesToEnd = StateMachine->FramesInState + EndLagFrames;
		ShieldComponent->SetShieldActive(false);
		InEndLag = true;
		FighterPawnRef->SetCurrentAnimation("ShieldRelease", 3);
		return true;
	}
	return false;
}

bool UShieldState::HandleStickInput(FFighterInput& Input)
{
	return false;
}

bool UShieldState::HandlePhysics(FFighterInput& Input)
{
	MoveComp->ApplyGroundFriction();
	MoveComp->PreventLedgeFall(false);
	if (MoveComp->IsAirborne())
	{
		FighterPawnRef->SetCurrentAnimation("Falling");
		StateMachine->TryChangeState("Falling", Input);
		return true;
	}
	return false;
}

void UShieldState::OnExit()
{
	ShieldComponent->SetShieldActive(false);
	InEndLag = false;
}