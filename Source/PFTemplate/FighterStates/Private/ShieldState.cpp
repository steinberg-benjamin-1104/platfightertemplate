#include "ShieldState.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.h"

void UShieldState::InitState(AFighterPawn* InFighterPawn, UFighterMovementComponent* InMoveComp, UFighterStateMachine* InStateMachine, FInputBuffer* InBuffer)
{
	Super::InitState(InFighterPawn, InMoveComp, InStateMachine, InBuffer);
	ShieldComponent = InFighterPawn->ShieldComponent;
}

void UShieldState::OnEnter()
{
	FighterPawnRef->SetCurrentAnimation("Shield");
}

bool UShieldState::HandleTimer(int32 FramesInState)
{
	if (FramesInState == StartupFrames)
	{
		ShieldComponent->SetShieldActive(true);
	}

	if (FramesInState >= FramesToEnd && InEndLag)
	{
		FighterPawnRef->SetCurrentAnimation("Idle");
		StateMachine->ChangeFighterState("Idle");
		return true;
	}
	return false;
}

void UShieldState::HandleInput()
{
	if (FFighterInput* Input = InputBuffer->WasPressed(EInputButton::Jump))
	{
		Input->Consume(EInputButton::Jump);
		StateMachine->ChangeFighterState("JumpSquat");
		return;
	}

	if (!InputBuffer->IsHeld(EInputButton::Shield))
	{
		if (!ShieldComponent->IsActive() || InEndLag) return;
		FramesToEnd = StateMachine->FramesInState + EndLagFrames;
		ShieldComponent->SetShieldActive(false);
		InEndLag = true;
		FighterPawnRef->SetCurrentAnimation("ShieldRelease", 3);
	}
}

bool UShieldState::HandlePhysics()
{
	MoveComp->ApplyGroundFriction();
	MoveComp->PreventLedgeFall(false);
	if (MoveComp->IsAirborne())
	{
		
		StateMachine->ChangeFighterState("Falling");
		return true;
	}
	return false;
}

void UShieldState::OnExit()
{
	ShieldComponent->SetShieldActive(false);
	InEndLag = false;
}