#include "FallingState.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.h"
#include "FixedCollision.h"
#include "Ledge.h"
#include "SafeMath.h"
#include "Engine/OverlapResult.h"

void UFallingState::OnEnter(FFighterInput& NewInput)
{
	MoveComp->CollisionCapsule.LiftBottom();
}

bool UFallingState::HandleButtonInput(FFighterInput& NewInput)
{
	FButtonState &ButtonState = NewInput.Button;
	
	if (ButtonState.IsPressed(EInputButton::Jump))
	{
		if (MoveComp->DoHop(EHopType::Air))
		{
			FFixedVector2D Velocity = MoveComp->GetVelocity();
			Velocity.X = NewInput.Stick.Current.X * MoveComp->MaxAirSpeed;
			MoveComp->SetVelocity(Velocity);
			
			StateMachine->TryChangeState("Rising", NewInput);
			return true;
		}
	}

	if (ButtonState.IsPressed(EInputButton::Shield) || ButtonState.IsHeld(EInputButton::Shield))
	{
		// air dodge
	}

	if (ButtonState.IsPressed(EInputButton::Attack))
	{
		return FighterPawnRef->TryStartAttack(EInputButton::Attack, NewInput);
	}

	if (ButtonState.IsPressed(EInputButton::Special))
	{
		return FighterPawnRef->TryStartAttack(EInputButton::Special, NewInput);
	}

	return false;
}

bool UFallingState::HandlePhysics(FFighterInput& Input)
{
	if (MoveComp->IsGrounded())
	{
		FighterPawnRef->SetCurrentAnimation("Landing");
		StateMachine->TryChangeState("Idle", Input);
		return true;
	}
	return false;
}

bool UFallingState::HandleStickInput(FFighterInput& Input)
{
	if (!MoveComp->bIsFastFalling && Input.Stick.bDownThisFrame)
	{
		MoveComp->bIsFastFalling = true;
	}
	
	MoveComp->ApplyAirDrift(Input.Stick.Current.X);

	ALedge* NearbyLedge = DetectNearbyLedge();
	if (NearbyLedge)
	{
		float IsRight = NearbyLedge->IsRight ? 1.0f : -1.0f;
		FighterPawnRef->SetFacingDirection(IsRight * -1.f);
		FighterPawnRef->SetActorLocation(NearbyLedge->GetActorLocation() + FVector(25.f * IsRight, 0.f, -200.f));
		FighterPawnRef->StateMachine->TryChangeState("OnLedge", Input);
		return true;
	}

	return false;
}

void UFallingState::OnExit()
{
	MoveComp->bIsFastFalling = false;
}

ALedge* UFallingState::DetectNearbyLedge() const
{
	const FFixedVector2D BaseLoc = FighterPawnRef->GetFixedLoc() + MoveComp->GetVelocity()/FixedDt;
	const int32 ForwardDir = FighterPawnRef->GetFacingDirection();
	const FFixedVector2D HalfExtent(60.f, 40.f);
	
	const FFixedVector2D BoxCenter = BaseLoc + FFixedVector2D(ForwardDir * 30, 150.f);
	
	//DrawDebugBox(World, BoxCenter, HalfExtent, FQuat::Identity, FColor::Green, false, -1.f, 0, 2.f);
	
	return FixedOverlapBoxFirstActorOfClass<ALedge>(GetWorld(), BoxCenter, HalfExtent);
}