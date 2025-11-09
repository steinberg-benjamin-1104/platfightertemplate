#include "DashState.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.h"

void UDashState::InitState(AFighterPawn* InFighterPawn, UFighterMovementComponent* InMoveComp, UFighterStateMachine* InStateMachine)
{
	Super::InitState(InFighterPawn, InMoveComp, InStateMachine);
	DashDuration = MoveComp->DashDuration;
}

void UDashState::OnEnter()
{
	StartDash();
	FighterPawnRef->SetCurrentAction("Dash");
}

void UDashState::StartDash()
{
	const FStickInt8 StickInput = FighterPawnRef->GetPlayerStickInput();
	DashDirection = (StickInput.X > 0) ? 1.f : -1.f;
	
	FighterPawnRef->SetFacingDirection(DashDirection);
}

void UDashState::Tick()
{
	Super::Tick();

	const FStickInt8 StickInput = FighterPawnRef->GetPlayerStickInput();

	if (StickInput.X != 0)
	{
		const float StickDir = (StickInput.X > 0.f) ? 1.f : -1.f;
		if (StickDir != DashDirection)
			StartDash();
	}

	ApplyDashVelocity();
	MoveComp->HandleLedgeOrFall(false);

	if (MoveComp->IsAirborne())
	{
		FighterPawnRef->SetCurrentAction("Falling");
		FighterPawnRef->StateMachine->TryChangeState("Falling");
		
		return;
	}
	
	HandleDashEnd(StickInput);
}

void UDashState::ApplyDashVelocity()
{
	FVector Velocity = MoveComp->GetVelocity();
	Velocity.X = DashDirection * MoveComp->DashSpeed;
	MoveComp->SetVelocity(Velocity);
}

void UDashState::HandleDashEnd(const FStickInt8& StickInput)
{
	const int32 StickDir = FMath::Sign(StickInput.X);
	
	if (StateMachine->FramesInState == DashDuration)
	{
		if (StickDir == DashDirection)
		{
			StateMachine->TryChangeState("Run");
		}
		else
		{
			FighterPawnRef->SetCurrentAction("Idle", 3);
			StateMachine->TryChangeState("Idle");
		}
	}
}

void UDashState::OnExit()
{
	Super::OnExit();
}

bool UDashState::JumpPressed()
{
	return StateMachine->TryChangeState("JumpSquat");
}

bool UDashState::ShieldPressed()
{
	MoveComp->SetVelocity(FVector(0, 0, 0));
	return StateMachine->TryChangeState("Shield");
}

bool UDashState::Attack()
{
	FStickInputTracker& Tracker = *FighterPawnRef->GetStickTracker();
	
	if (StateMachine->FramesInState < 4)
	{
		FighterPawnRef->SetCurrentAction("Fsmash1");
		MoveComp->HaltHorizontalVelocity();
		return true;
	}

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