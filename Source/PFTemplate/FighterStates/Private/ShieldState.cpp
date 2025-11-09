#include "ShieldState.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.h"

void UShieldState::InitState(AFighterPawn* InFighterPawn, UFighterMovementComponent* InMoveComp, UFighterStateMachine* InStateMachine)
{
	Super::InitState(InFighterPawn, InMoveComp, InStateMachine);
	ShieldComponent = InFighterPawn->ShieldComponent;
}

void UShieldState::OnEnter()
{
	FighterPawnRef->SetCurrentAction("Shield", 5);
}

void UShieldState::Tick()
{
	MoveComp->ApplyGroundFriction();
	MoveComp->HandleLedgeOrFall(false);

	if (MoveComp->IsAirborne())
	{
		FighterPawnRef->StateMachine->TryChangeState("Falling");
		return;
	}

	if (StateMachine->FramesInState == StartupFrames)
	{
		ShieldComponent->SetShieldActive(true);
	}
	
	if (StateMachine->FramesInState >= StartupFrames + MinHoldFrames)
	{
		if (!FighterPawnRef->FPC->IsShieldHeld())
		{
			ShieldReleased();
		}
	}

	if (StateMachine->FramesInState >= FramesToEnd && InEndLag)
	{
		FighterPawnRef->StateMachine->TryChangeState("Idle");
		return;
	}
	
	// FStickInputTracker& Tracker = *FighterPawnRef->GetStickTracker();
	// Tracker.CaptureStickDir();
	// EStickDirection Dir = Tracker.GetCapturedDir();
	//
	// if (Tracker.isFlickHorizontal())
	// {
	// 	const float Facing = FighterPawnRef->GetFacingDirection();
	// 	bool bRollForward = (Dir == EStickDirection::Right && Facing > 0.f) ||
	// 						(Dir == EStickDirection::Left && Facing < 0.f);
	//
	// 	if (bRollForward)
	// 	{
	// 		FighterPawnRef->SetFacingDirection(-Facing);
	// 		FighterPawnRef->SetCurrentAction("FRoll");
	// 	}
	// 	else
	// 	{
	// 		FighterPawnRef->SetFacingDirection(Facing);
	// 		FighterPawnRef->SetCurrentAction("BRoll");
	// 	}
	// 	StateMachine->TryChangeState("Attacking");
	// 	MoveComp->bCanApplyGroundFriction = false;
	// 	return;
	// }
	//
	// if (Tracker.WasDownJustPressedThisFrame())
	// {
	// 	MoveComp->HaltHorizontalVelocity();
	// 	FighterPawnRef->SetCurrentAction("Parry"); //no spotdodge in this game
	// 	StateMachine->TryChangeState("Attacking");
	// }
}

bool UShieldState::JumpPressed()
{
	return StateMachine->TryChangeState("JumpSquat");
}

bool UShieldState::ShieldReleased()
{
	if (!ShieldComponent->IsActive() || InEndLag) return false;
	FramesToEnd = StateMachine->FramesInState + EndLagFrames;
	ShieldComponent->SetShieldActive(false);
	InEndLag = true;
	FighterPawnRef->SetCurrentAction("ShieldRelease", 3);
	return true;
}

void UShieldState::OnExit()
{
	ShieldComponent->SetShieldActive(false);
	InEndLag = false;
}