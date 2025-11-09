#include "IdleState.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.h"

void UIdleState::OnExit()
{
	bOnLedge = false;
}

void UIdleState::Tick()
{
	MoveComp->ApplyGroundFriction();
	MoveComp->HandleLedgeOrFall(false);

	if (MoveComp->IsAirborne())
	{
		FighterPawnRef->SetCurrentAction("Falling");
		FighterPawnRef->StateMachine->TryChangeState("Falling");
		return;
	}
	
	if (!FighterPawnRef->GetStickTracker()) return;
	
	if (FighterPawnRef->GetStickTracker()->isFlickHorizontal())
	{
		FighterPawnRef->StateMachine->TryChangeState("Dash");
		return;
	}

	const FStickInt8 StickInput = FighterPawnRef->GetPlayerStickInput();
	const int8 AbsStickX = FMath::Abs(StickInput.X);
	constexpr int8 WalkThreshold = 13;

	if (AbsStickX > WalkThreshold)
	{
		StateMachine->TryChangeState("Walking");
		return;
	}

	if (MoveComp->IsStandingOnFacingLedge() && FMath::IsNearlyZero(MoveComp->GetVelocity().X) && !bOnLedge)
	{
		FighterPawnRef->SetCurrentAction("Teeter");
		bOnLedge = true;
	}
}

bool UIdleState::JumpPressed()
{
	return StateMachine->TryChangeState("JumpSquat");
}

bool UIdleState::Attack()
{
	FStickInputTracker& Tracker = *FighterPawnRef->GetStickTracker();
	Tracker.CaptureStickDir();
	EStickDirection Dir = Tracker.GetCapturedDir();
	FName MoveName;

	if (Tracker.IsFlickActive()) // Handle smash attacks
	{
		switch (Dir)
		{
		case EStickDirection::Neutral: MoveName = "Fsmash1"; break;
		case EStickDirection::Up:      MoveName = "Upsmash1"; break;
		case EStickDirection::Down:    MoveName = "Dsmash1"; break;
		case EStickDirection::Left:
			FighterPawnRef->SetFacingDirection(-1.f);
			MoveName = "Fsmash1";
			break;
		case EStickDirection::Right:
			FighterPawnRef->SetFacingDirection(1.f);
			MoveName = "Fsmash1";
			break;
		default: MoveName = "Fsmash1"; break;
		}
	}
	else
	{
		switch (Dir)
		{
		case EStickDirection::Neutral: MoveName = "Jab1"; break;
		case EStickDirection::Up:      MoveName = "Uptilt1"; break;
		case EStickDirection::Down:    MoveName = "Dtilt1"; break;
		case EStickDirection::Left:
			FighterPawnRef->SetFacingDirection(-1.f);
			MoveName = "Ftilt1";
			break;
		case EStickDirection::Right:
			FighterPawnRef->SetFacingDirection(1.f);
			MoveName = "Ftilt1";
			break;
		default: MoveName = "Jab1"; break;
		}
	}
	
	return FighterPawnRef->SetCurrentAction(MoveName);
}

bool UIdleState::ShieldPressed()
{
	return StateMachine->TryChangeState("Shield");
}