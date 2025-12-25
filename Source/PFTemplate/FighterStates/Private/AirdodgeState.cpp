#include "AirdodgeState.h"
#include "FighterMovementComponent.h"
#include "FighterPawn.h"

void UAirDodgeState::OnEnter(FFighterInput& Input)
{
	FFixedVector2D StickDir = Input.Stick.StickPos;
	bIsGrounded = false;
	FFixedVector2D Velocity = Speed * StickDir;
	MoveComp->SetVelocity(Velocity);
	MoveComp->SetMovementMode(EFighterMovementMode::None);
	FighterPawnRef->SetCurrentAnimation("Airdodge", 6);
}

bool UAirDodgeState::HandleTimer(FFighterInput& Input, int32 FramesInState)
{
	if (FramesInState == MvmtDuration)
	{
		if (bIsGrounded)
		{
			StateMachine->ChangeFighterState("Idle", Input);
			return true;
		}
		MoveComp->SetVelocity(FFixedVector2D());
	}
	if (FramesInState == Duration)
	{
		MoveComp->SetMovementMode(EFighterMovementMode::Falling);
		FighterPawnRef->SetCurrentAnimation("Falling");
		StateMachine->ChangeFighterState("Falling", Input);
		return true;
	}
	return false;
}

bool UAirDodgeState::HandlePhysics(FFighterInput& Input)
{
	if (MoveComp->IsGrounded() && !bIsGrounded)
	{
		bIsGrounded = true;
		FighterPawnRef->SetCurrentAnimation("Landing");
	}
	if (bIsGrounded)
	{
		MoveComp->ApplyGroundFriction();
		MoveComp->PreventLedgeFall(false);
		if (MoveComp->CurrentMovementMode == EFighterMovementMode::Falling)
		{
			FighterPawnRef->SetCurrentAnimation("Falling");
			StateMachine->ChangeFighterState("Falling", Input);
			return true;
		}
	}
	return false;
}