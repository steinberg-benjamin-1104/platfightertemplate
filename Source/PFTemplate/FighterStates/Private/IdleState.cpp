#include "IdleState.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.h"

bool UIdleState::HandlePhysics()
{
	MoveComp->ApplyGroundFriction();
	MoveComp->PreventLedgeFall(false);
	
	if (MoveComp->IsAirborne())
	{
		FighterPawnRef->StateMachine->ChangeFighterState("Falling");
		return true;
	}

	return false;
}

void UIdleState::HandleInput()
{

	// Handle Attack Buttons Here!!
	
	if (FFighterInput* Input = InputBuffer->WasPressed(EInputButton::Jump))
	{
		Input->Consume(EInputButton::Jump);
		StateMachine->ChangeFighterState("JumpSquat");
	}

	if (FFighterInput* Input = InputBuffer->WasPressed(EInputButton::Shield))
	{
		Input->Consume(EInputButton::Jump);
		StateMachine->ChangeFighterState("ShieldStartup");
	}

	if (InputBuffer->IsHeld(EInputButton::Shield))
	{
		StateMachine->ChangeFighterState("ShieldStartup");
		return;
	}

	if (FFighterInput* Input = InputBuffer->WasPressed(EInputButton::StickDown))
	{
		if (MoveComp->PhysicsSnapshot.bOnPlatform)
		{
			Input->Consume(EInputButton::StickDown);
			StateMachine->ChangeFighterState("PlatformDrop");
		}
	}

	EStickDir Stickdir = GetCurrentStickDir();
	if (Stickdir == EStickDir::Backward || Stickdir == EStickDir::Forward)
	{
		if (InputBuffer->GetRecent()->IsPressed(EInputButton::Flick))
		{
			InputBuffer->GetRecent()->Consume(EInputButton::Flick);
			FighterPawnRef->StateMachine->ChangeFighterState("Dash");
		}
		else
		{
			StateMachine->ChangeFighterState("Walking");
		}
	}
}