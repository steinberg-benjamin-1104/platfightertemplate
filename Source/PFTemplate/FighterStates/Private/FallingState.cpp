#include "FallingState.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.h"
#include "FixedCollision.h"
#include "Ledge.h"
#include "SafeMath.h"

void UFallingState::OnEnter()
{
	FighterPawnRef->SetCurrentAnimation("Falling");
}

void UFallingState::HandleInput()
{
	if (CheckActionButtons()) return;
	
	if (FFighterInput* Input = InputBuffer->WasPressed(EInputButton::Jump))
	{
		if (MoveComp->StartJump(EJumpType::Air))
		{
			FFixedVector2D Velocity = MoveComp->GetVelocity();
			Velocity.X = InputBuffer->GetRecent().StickPos.X * MoveComp->MaxAirSpeed;
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

	MoveComp->ApplyAirDrift(InputBuffer->GetRecent().StickPos.X);
}

bool UFallingState::HandlePhysics()
{
	if (MoveComp->IsGrounded())
	{
		FighterPawnRef->SetCurrentAnimation("Landing");
		StateMachine->ChangeFighterState("Idle");
		return true;
	}

	return false;
}

void UFallingState::Tick(int32 FramesInState)
{
	Super::Tick(FramesInState);

	if (ALedge* NearbyLedge = DetectNearbyLedge())
	{
		FFixedVector2D LedgePos = VectorToFixed2D(NearbyLedge->GetActorLocation());
		int32 IsRight = NearbyLedge->IsRight ? 1 : -1;
		FighterPawnRef->SetFacingDirection(IsRight * -1);
		FighterPawnRef->SetFixedLoc(LedgePos + FFixedVector2D(FFixed_32(25.f) * IsRight, -200.f ));
		FighterPawnRef->StateMachine->ChangeFighterState("OnLedge");
	}
}

ALedge* UFallingState::DetectNearbyLedge() const
{
	const FFixedVector2D BaseLoc = FighterPawnRef->GetFixedLoc() + MoveComp->GetVelocity() * FixedDt;
	const int32 ForwardDir = FighterPawnRef->GetFacingDirection();
	const FFixedVector2D HalfExtent(60.f, 40.f);
	
	const FFixedVector2D BoxCenter = BaseLoc + FFixedVector2D(ForwardDir * 30, 150.f);
	
	DrawDebugBox(GetWorld(), Fixed2DToVector(BoxCenter), Fixed2DToVector(HalfExtent), FQuat::Identity, FColor::Green, false, -1.f, 0, 2.f);
	
	return FixedOverlapBoxFirstActorOfClass<ALedge>(GetWorld(), BoxCenter, HalfExtent);
}