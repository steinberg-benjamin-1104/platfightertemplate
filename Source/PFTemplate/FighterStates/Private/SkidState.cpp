#include "SkidState.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.h"

void USkidState::OnEnter()
{
	FighterPawnRef->FlipFacingDirection();
	FighterPawnRef->SetCurrentAnimation("Skid");
	Reduction = MoveComp->GetVelocity().X.Abs() / SkidDuration;
}

bool USkidState::HandleTimer(int32 FramesInState)
{
	if (FramesInState == SkidDuration)
	{
		FighterPawnRef->SetCurrentAnimation("Idle", 3);
		StateMachine->ChangeFighterState("Idle");
		return true;
	}
	return false;
}

bool USkidState::HandlePhysics()
{
	FFixedVector2D Velocity = MoveComp->GetVelocity();
	Velocity.X -= Reduction * MoveComp->GetVelocity().X.Sign();
	MoveComp->SetVelocity(Velocity);
	MoveComp->PreventLedgeFall(true);
	return false;
}

void USkidState::HandleInput()
{
	static const TMap<EInputButton, FName> ButtonToState = {
		{ EInputButton::Jump, "JumpSquat" },
		{ EInputButton::StickDown, "PlatformDrop" },
		{ EInputButton::Shield, "Shield"}
	};

	if (CheckBufferedButtonStateChanges(ButtonToState)) return;

	if (InputBuffer->IsHeld(EInputButton::Shield))
	{
		StateMachine->ChangeFighterState("Shield");
	}
}