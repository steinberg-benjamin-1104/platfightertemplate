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
	ShieldComponent->SetShieldActive(true);
}

void UShieldState::HandleInput()
{
	if (FFighterInput* Input = InputBuffer->WasPressed(EInputButton::Jump))
	{
		Input->Consume(EInputButton::Jump);
		StateMachine->ChangeFighterState("JumpSquat");
		return;
	}

	if (!InputBuffer->IsHeld(EInputButton::Shield) && StateMachine->FramesInState)
	{
		ShieldComponent->SetShieldActive(false);
		StateMachine->ChangeFighterState("ShieldRelease");
	}

	EStickDir StickDir = GetCurrentStickDir();

	if (StickDir == EStickDir::Forward && InputBuffer->WasPressed(EInputButton::Flick))
	{
		FighterPawnRef->FlipFacingDirection();
		if (FighterPawnRef->SetCurrentAnimation("ForwardRoll"))
			StateMachine->ChangeFighterState("MiscAnim");
		return;
	}

	if (StickDir == EStickDir::Backward && InputBuffer->WasPressed(EInputButton::Flick))
	{
		if (FighterPawnRef->SetCurrentAnimation("BackRoll"))
			StateMachine->ChangeFighterState("MiscAnim");
		return;
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
}