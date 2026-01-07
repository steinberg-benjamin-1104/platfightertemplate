#include "AirdodgeState.h"
#include "FighterMovementComponent.h"
#include "FighterPawn.h"

void UAirDodgeState::OnEnter()
{
	FFixedVector2D StickPos = InputBuffer->GetRecent()->StickPos;
	bIsGrounded = false;
	FFixedVector2D Velocity = Speed * StickPos.GetSafeNormal();
	MoveComp->SetVelocity(Velocity);
	MoveComp->SetMovementMode(EFighterMovementMode::None);
	FighterPawnRef->SetCurrentAnimation("Airdodge", 6);
}

bool UAirDodgeState::HandleTimer(int32 FramesInState)
{
	if (FramesInState == MvmtDuration)
	{
		if (bIsGrounded)
		{
			StateMachine->ChangeFighterState("Idle");
			return true;
		}
		MoveComp->SetVelocity(FFixedVector2D());
	}
	if (FramesInState == Duration)
	{
		MoveComp->SetMovementMode(EFighterMovementMode::Falling);
		StateMachine->ChangeFighterState("Falling");
		return true;
	}
	return false;
}

bool UAirDodgeState::HandlePhysics()
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
			StateMachine->ChangeFighterState("Falling");
			return true;
		}
	}
	return false;
}