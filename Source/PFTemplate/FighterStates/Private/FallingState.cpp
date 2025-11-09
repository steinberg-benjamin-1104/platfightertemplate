#include "FallingState.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.h"
#include "Ledge.h"
#include "Engine/OverlapResult.h"

void UFallingState::OnEnter()
{
	MoveComp->CollisionCapsule.LiftBottom();
}

void UFallingState::Tick()
{
	Super::Tick();
	
	if (MoveComp->IsGrounded())
	{
		OnLand();
		return;
	}

	// Fast-fall detection
	if (!MoveComp->bIsFastFalling)
	{
		FStickInputTracker* Tracker = FighterPawnRef->GetStickTracker();
		if (Tracker && Tracker->WasDownJustPressedThisFrame())
		{
			MoveComp->bIsFastFalling = true;
		}
	}

	ALedge* NearbyLedge = DetectNearbyLedge();
	if (NearbyLedge)
	{
		float IsRight = NearbyLedge->IsRight ? 1.0f : -1.0f;
		FighterPawnRef->SetFacingDirection(IsRight * -1.f);
		FighterPawnRef->SetActorLocation(NearbyLedge->GetActorLocation() + FVector(25.f * IsRight, 0.f, -200.f));
		FighterPawnRef->StateMachine->TryChangeState("OnLedge");
		return;
	}

	const float StickX = FighterPawnRef->GetPlayerStickInput().ToFloat().X;
	MoveComp->ApplyAirDrift(StickX);
}

bool UFallingState::Attack()
{
	FStickInputTracker& Tracker = *FighterPawnRef->GetStickTracker();
	Tracker.CaptureStickDir();

	EStickDirection Dir = Tracker.GetCapturedDir();
	FName MoveName;

	switch (Dir)
	{
		case EStickDirection::Neutral: MoveName = "Nair1"; break;
		case EStickDirection::Up:      MoveName = "Upair1"; break;
		case EStickDirection::Down:    MoveName = "Dair1"; break;
		case EStickDirection::Left:
			MoveName = FighterPawnRef->IsFacingRight()? "Bair1" : "Fair1"; break;
		case EStickDirection::Right:
			MoveName = FighterPawnRef->IsFacingRight()? "Fair1" : "Bair1"; break;
		default: MoveName = "Nair1"; break;
	}
	FighterPawnRef->SetCurrentAction(MoveName);
	return true;
}

bool UFallingState::JumpPressed()
{
	if (MoveComp->DoHop(EHopType::Air))
	{
		const float StickX = FighterPawnRef->GetPlayerStickInput().X;
		const float NormalizedX = FMath::Clamp(StickX, -1.f, 1.f);

		if (!FMath::IsNearlyZero(NormalizedX))
		{
			FVector Velocity = MoveComp->Velocity;
			Velocity.X = NormalizedX * MoveComp->MaxAirSpeed;
			MoveComp->Velocity = Velocity;
		}

		StateMachine->TryChangeState("Rising");
		return true;
	}
	return false;
}

void UFallingState::OnExit()
{
	Super::OnExit();
	MoveComp->bIsFastFalling = false;
}

ALedge* UFallingState::DetectNearbyLedge() const
{
	UWorld* World = FighterPawnRef->GetWorld();
	if (!World) return nullptr;

	// Head position offset (200 up, 30 forward/backward reach)
	const FVector BaseLoc   = FighterPawnRef->GetActorLocation() + FighterPawnRef->MovementComponent->GetVelocity()/60.f;
	const float ForwardDir  = FighterPawnRef->GetFacingDirection();
	
	const FVector HalfExtent(60.f, 1.f, 40.f);

	// Place the box slightly forward, and at head height
	const FVector BoxCenter = BaseLoc + FVector(ForwardDir * 30.f, 0.f, 150.f);

	FCollisionShape BoxShape = FCollisionShape::MakeBox(HalfExtent);
	FCollisionQueryParams Params(SCENE_QUERY_STAT(LedgeCheck), false, FighterPawnRef);
	
	//DrawDebugBox(World, BoxCenter, HalfExtent, FQuat::Identity, FColor::Green, false, -1.f, 0, 2.f);
	
	TArray<FOverlapResult> Overlaps;
	bool bAny = World->OverlapMultiByObjectType(
		Overlaps,
		BoxCenter,
		FQuat::Identity,
		FCollisionObjectQueryParams(ECC_WorldDynamic),
		BoxShape,
		Params
	);

	if (bAny)
	{
		for (const FOverlapResult& Overlap : Overlaps)
		{
			if (ALedge* Ledge = Cast<ALedge>(Overlap.GetActor()))
			{
				return Ledge;
			}
		}
	}
	return nullptr;
}

void UFallingState::OnLand()
{
	FighterPawnRef->SetCurrentAction("Landing");
	StateMachine->TryChangeState("Idle");
}