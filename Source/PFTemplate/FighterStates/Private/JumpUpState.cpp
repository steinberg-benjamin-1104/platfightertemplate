#include "JumpUpState.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.h"

void UJumpUpState::OnEnter()
{
	FighterPawnRef->SetCurrentAnimation("JumpUp");
}

bool UJumpUpState::HandleTimer(int32 FramesInState)
{
	if (FramesInState == Duration) MoveComp->StartGroundJump();
	return false;
}

void UJumpUpState::HandleInput()
{
	if (CheckActionButtons()) return;
	
	if (FFighterInput* Input = InputBuffer->WasPressed(EInputButton::Jump))
	{
		if (MoveComp->StartJump(EJumpType::Air))
		{
			FFixedVector2D Velocity = MoveComp->GetVelocity();
			Velocity.X = InputBuffer->GetRecent()->StickPos.X * MoveComp->MaxAirSpeed;
			MoveComp->SetVelocity(Velocity);

			Input->Consume(EInputButton::Jump);
			StateMachine->ChangeFighterState("JumpUp");
			return;
		}
	}

	if (FFighterInput* Input = InputBuffer->WasPressed(EInputButton::Shield))
	{
		Input->Consume(EInputButton::Jump);
		StateMachine->ChangeFighterState("Airdodge");
	}

	MoveComp->ApplyAirDrift(InputBuffer->GetRecent()->StickPos.X);
}

bool UJumpUpState::HandlePhysics()
{
	if (MoveComp->GetCurrentMode() == EFighterMovementMode::Falling)
	{
		StateMachine->ChangeFighterState("Falling");
		return true;
	}
	return false;
}