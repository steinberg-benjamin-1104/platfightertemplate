#include "IdleState.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.h"

void UIdleState::OnExit()
{
	bOnLedge = false;
}

bool UIdleState::HandlePhysics()
{
	MoveComp->ApplyGroundFriction();
	MoveComp->PreventLedgeFall(false);
	
	if (MoveComp->IsAirborne())
	{
		FighterPawnRef->StateMachine->ChangeFighterState("Falling");
		return true;
	}

	if (MoveComp->IsStandingOnFacingLedge() && (MoveComp->GetVelocity().X == FFixed_32(0.f)) && !bOnLedge)
	{
		FighterPawnRef->SetCurrentAnimation("Teeter");
		bOnLedge = true;
	}

	return false;
}

void UIdleState::HandleInput()
{
	if (CheckActionButtons()) return;
	
	static const TMap<EInputButton, FName> ButtonToState = {
		{ EInputButton::Jump, "JumpSquat" },
		{ EInputButton::Shield, "ShieldStartup"}
	};

	if (CheckBufferedButtonStateChanges(ButtonToState)) return;

	if (InputBuffer->IsHeld(EInputButton::Shield))
	{
		StateMachine->ChangeFighterState("ShieldStartup");
		return;
	}

	if (FFighterInput* Input = InputBuffer->WasPressed(EInputButton::StickDown))
	{
		if (MoveComp->bOnPlatform)
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