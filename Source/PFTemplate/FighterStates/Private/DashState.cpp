#include "DashState.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.h"
#include "StickDirection.h"
#include "AttackDefinition.h"

void UDashState::InitState(AFighterPawn* InFighterPawn, UFighterMovementComponent* InMoveComp, UFighterStateMachine* InStateMachine, FInputBuffer* InBuffer)
{
	Super::InitState(InFighterPawn, InMoveComp, InStateMachine, InBuffer);
	DashDuration = MoveComp->DashDuration;
}

void UDashState::OnEnter()
{
	FighterPawnRef->SetCurrentAnimation("Dash");

	EStickDir StickDir = GetStickDirection(InputBuffer->GetRecent().StickPos, FighterPawnRef->IsFacingRight());
	
	if (StickDir == EStickDir::Backward) FighterPawnRef->FlipFacingDirection();
	FFixedVector2D Velocity = MoveComp->GetVelocity();
	Velocity.X = FighterPawnRef->GetFacingDirection() * MoveComp->DashSpeed.ToFixed();
	MoveComp->SetVelocity(Velocity);
}

bool UDashState::HandlePhysics()
{
	MoveComp->PreventLedgeFall(false);

	if (MoveComp->IsAirborne())
	{
		FighterPawnRef->StateMachine->ChangeFighterState("Falling");
		return true;
	}
	
	return false;
}

void UDashState::HandleInput()
{
	if (CheckActionButtons()) return;
	
	static const TMap<EInputButton, FName> ButtonToState = {
		{ EInputButton::Jump, "JumpSquat" },
	};

	if (CheckBufferedButtonStateChanges(ButtonToState)) return;

	if (FFighterInput* Input = InputBuffer->WasPressed(EInputButton::Shield))
	{
		MoveComp->HaltHorizontalVelocity();
		Input->Consume(EInputButton::Shield);
		StateMachine->ChangeFighterState("Shield");
		return;
	}
	
	if (InputBuffer->IsHeld(EInputButton::Shield))
	{
		MoveComp->HaltHorizontalVelocity();
		StateMachine->ChangeFighterState("Shield");
		return;
	}

	if (FFighterInput* Input = InputBuffer->WasPressed(EInputButton::StickDown))
	{
		if (MoveComp->bOnPlatform)
		{
			Input->Consume(EInputButton::StickDown);
			StateMachine->ChangeFighterState("PlatformDrop");
			return;
		}
	}

	EStickDir StickDir = GetCurrentStickDir();
	
	if (StickDir == EStickDir::Backward && InputBuffer->GetRecent().IsPressed(EInputButton::Flick))
	{
		InputBuffer->GetRecent().Consume(EInputButton::Flick);
		StateMachine->ChangeFighterState("Dash");
	}
}

bool UDashState::HandleTimer(int32 FramesInState)
{
	if (FramesInState == DashDuration)
	{
		EStickDir StickDir = GetCurrentStickDir();
		if (StickDir == EStickDir::Forward)
		{
			StateMachine->ChangeFighterState("Run");
			return true;
		}
		
		if (StickDir == EStickDir::Backward && InputBuffer->GetRecent().IsPressed(EInputButton::Flick))
		{
			StateMachine->ChangeFighterState("Dash");
			return true;
		}
		
		FighterPawnRef->SetCurrentAnimation("Idle", 3);
		StateMachine->ChangeFighterState("Idle");
		MoveComp->HaltHorizontalVelocity();
	}

	return false;
}