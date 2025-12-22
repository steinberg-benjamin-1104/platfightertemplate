#include "FallingState.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.h"
#include "FixedCollision.h"
#include "Ledge.h"
#include "SafeMath.h"

bool UFallingState::HandleButtonInput(FFighterInput& NewInput)
{
	FButtonState &ButtonState = NewInput.Button;
	
	if (ButtonState.IsPressed(EInputButton::Jump))
	{
		if (MoveComp->DoHop(EHopType::Air))
		{
			FFixedVector2D Velocity = MoveComp->GetVelocity();
			Velocity.X = NewInput.Stick.StickPos.X * MoveComp->MaxAirSpeed;
			MoveComp->SetVelocity(Velocity);
			
			StateMachine->ChangeFighterState("Rising", NewInput);
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
		StateMachine->ChangeFighterState("Idle", Input);
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
	
	MoveComp->ApplyAirDrift(Input.Stick.StickPos.X);
	
	return false;
}

void UFallingState::Tick(FFighterInput& Input, int32 FramesInState)
{
	Super::Tick(Input, FramesInState);

	if (ALedge* NearbyLedge = DetectNearbyLedge())
	{
		FFixedVector2D LedgePos = VectorToFixed2D(NearbyLedge->GetActorLocation());
		int32 IsRight = NearbyLedge->IsRight ? 1 : -1;
		FighterPawnRef->SetFacingDirection(IsRight * -1);
		FighterPawnRef->SetFixedLoc(LedgePos + FFixedVector2D(FFixed_32(25.f) * IsRight, -200.f ));
		FighterPawnRef->StateMachine->ChangeFighterState("OnLedge", Input);
	}
}


void UFallingState::OnExit()
{
	MoveComp->bIsFastFalling = false;
}

ALedge* UFallingState::DetectNearbyLedge() const
{
	UE_LOG(LogTemp, Warning, TEXT("Detect nearby ledge"));
	const FFixedVector2D BaseLoc = FighterPawnRef->GetFixedLoc() + MoveComp->GetVelocity() * FixedDt;
	const int32 ForwardDir = FighterPawnRef->GetFacingDirection();
	const FFixedVector2D HalfExtent(60.f, 40.f);
	
	const FFixedVector2D BoxCenter = BaseLoc + FFixedVector2D(ForwardDir * 30, 150.f);
	
	DrawDebugBox(GetWorld(), Fixed2DToVector(BoxCenter), Fixed2DToVector(HalfExtent), FQuat::Identity, FColor::Green, false, -1.f, 0, 2.f);
	UE_LOG(LogTemp, Warning, TEXT("Drew New Box"));
	
	return FixedOverlapBoxFirstActorOfClass<ALedge>(GetWorld(), BoxCenter, HalfExtent);
}