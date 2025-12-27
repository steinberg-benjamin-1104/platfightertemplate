#include "IdleState.h"
#include "FighterPawn.h"
#include "AttackDefinition.h"
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
		FighterPawnRef->SetCurrentAnimation("Falling");
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

bool UIdleState::HandleButtonInput()
{
	
	if (InputBuffer->WasPressed(EInputButton::Jump))
	{
		InputBuffer->Consume(EInputButton::Jump);
		StateMachine->ChangeFighterState("JumpSquat");
		return true;
	}

	if (InputBuffer->WasPressed(EInputButton::Shield) || InputBuffer->IsHeld(EInputButton::Shield))
	{
		InputBuffer->Consume(EInputButton::Shield);
		StateMachine->ChangeFighterState("Shield");
		return true;
	}

	if (InputBuffer->WasPressed(EInputButton::Attack))
	{
		return FighterPawnRef->TryStartAttack(EInputButton::Attack);
	}

	if (ButtonState.IsPressed(EInputButton::Special))
	{
		return FighterPawnRef->TryStartAttack(EInputButton::Special, NewInput);
	}

	if (NewInput.Stick.bDownThisFrame && MoveComp->bOnPlatform)
	{
		StateMachine->ChangeFighterState("PlatformDrop", NewInput);
	}

	return false;
}

bool UIdleState::HandleStickInput(FFighterInput& NewInput)
{
	FStickState Stick = NewInput.Stick;
	EStickDir Stickdir = GetStickDirection(Stick.StickPos, FighterPawnRef->IsFacingRight());
	
	bool X = (Stickdir == EStickDir::Backward || Stickdir == EStickDir::Forward);
	if (Stick.bFlick && X)
	{
		NewInput.Stick.ConsumeFlick();
		FighterPawnRef->StateMachine->ChangeFighterState("Dash", NewInput);
		return true;
	}
	
	if (X)
	{
		StateMachine->ChangeFighterState("Walking", NewInput);
		return true;
	}

	return false;
}