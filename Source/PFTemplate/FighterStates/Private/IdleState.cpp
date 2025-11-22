#include "IdleState.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.h"

void UIdleState::OnExit()
{
	bOnLedge = false;
}

bool UIdleState::HandlePhysics(FFighterInput &NewInput)
{

	MoveComp->ApplyGroundFriction();
	MoveComp->PreventLedgeFall(false);
	
	if (MoveComp->IsAirborne())
	{
		FighterPawnRef->SetCurrentAction("Falling");
		FighterPawnRef->StateMachine->TryChangeState("Falling", NewInput);
		return true;
	}

	if (MoveComp->IsStandingOnFacingLedge() && (MoveComp->GetVelocity().X == FIXED_32(0.f)) && !bOnLedge)
	{
		FighterPawnRef->SetCurrentAction("Teeter");
		bOnLedge = true;
	}

	return false;
}

bool UIdleState::HandleButtonInput(FFighterInput &NewInput)
{
	if (NewInput.IsPressed(EInputButton::Jump))
	{
		StateMachine->TryChangeState("JumpSquat", NewInput);
		return true;
	}

	if (NewInput.IsPressed(EInputButton::Shield) || NewInput.IsHeld(EInputButton::Shield))
	{
		StateMachine->TryChangeState("Shield", NewInput);
		return true;
	}

	if (NewInput.IsPressed(EInputButton::Attack))
	{
		StateMachine->TryChangeState("GroundAttack", NewInput); //possibly need to update
		return true;
	}

	return false;
}

bool UIdleState::HandleStickInput(FFighterInput& NewInput)
{
	// if (!FighterPawnRef->GetStickTracker()) return;
	//
	// if (FighterPawnRef->GetStickTracker()->isFlickHorizontal())
	// {
	// 	FighterPawnRef->StateMachine->TryChangeState("Dash");
	// 	return;
	// }
	// 	const FStickInt8 StickInput = FighterPawnRef->GetPlayerStickInput();
	// 	const int8 AbsStickX = FMath::Abs(StickInput.X);
	// 	constexpr int8 WalkThreshold = 13;
	//
	// 	if (AbsStickX > WalkThreshold)
	// 	{
	// 		StateMachine->TryChangeState("Walking");
	// 		return;
	// 	}

	return false;
}