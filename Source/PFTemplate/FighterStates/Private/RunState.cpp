#include "RunState.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.h"

void URunState::OnEnter()
{
	FighterPawnRef->SetCurrentAction("Run");
}

void URunState::Tick()
{
	ApplyRunVelocity(MoveComp);
	MoveComp->HandleLedgeOrFall(false);

	if (MoveComp->GetCurrentMode() == EFighterMovementMode::Falling)
	{
		FighterPawnRef->SetCurrentAction("Falling");
		FighterPawnRef->StateMachine->TryChangeState("Falling");
		return;
	}

	HandleInputTransitions(MoveComp);
}

void URunState::ApplyRunVelocity(UFighterMovementComponent* MoveComp)
{
	const float Direction = FighterPawnRef->IsFacingRight() ? 1.f : -1.f;
	FVector Velocity = MoveComp->GetVelocity();
	Velocity.X = Direction * MoveComp->RunSpeed;
	MoveComp->SetVelocity(Velocity);
}

void URunState::HandleInputTransitions(UFighterMovementComponent* MoveComp)
{
	const FStickInt8 Stick = FighterPawnRef->GetPlayerStickInput();
	const float Direction = FighterPawnRef->GetFacingDirection();

	const bool bHoldingForward = (Stick.X * Direction) > 0;
	const bool bHoldingOpposite = (Stick.X * Direction) < 0;

	if (!bHoldingForward && !bHoldingOpposite)
	{
		FighterPawnRef->SetCurrentAction("Idle", 3);
		StateMachine->TryChangeState("Idle");
	}
	else if (bHoldingOpposite)
	{
		StateMachine->TryChangeState("Skid");
	}
}

bool URunState::JumpPressed()
{
	return StateMachine->TryChangeState("JumpSquat");
}

bool URunState::ShieldPressed()
{
	return StateMachine->TryChangeState("Shield");
}

bool URunState::Attack()
{
	FStickInputTracker& Tracker = *FighterPawnRef->GetStickTracker();
	
	// Up-flick: perform up smash
	if (Tracker.IsFlickActive() && Tracker.GetCapturedDir() == EStickDirection::Up)
	{
		FighterPawnRef->SetCurrentAction("Upsmash1");
		return true;
	}
	
	FighterPawnRef->SetCurrentAction("DashAttack1");
	MoveComp->bCanApplyGroundFriction = false;

	return true;
}