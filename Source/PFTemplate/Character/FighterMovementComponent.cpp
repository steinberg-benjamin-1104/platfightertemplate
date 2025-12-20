#include "FighterMovementComponent.h"
#include "GameFramework/Actor.h"
#include "DrawDebugHelpers.h"
#include "FighterPawn.h"
#include "FixedCollision.h"
#include "LandscapeRender.h"
#include "VectorUtil.h"

UFighterMovementComponent::UFighterMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UFighterMovementComponent::InitFMC(AFighterPawn* InFighterPawn)
{
	FighterPawnRef = InFighterPawn;
	CollisionCapsule.Reset();
	CollisionCapsule.UpdateCenter(FighterPawnRef->GetFixedLoc());
}

void UFighterMovementComponent::TickFMC()
{
	if (!FighterPawnRef) return;
	
	FollowCapsule = CollisionCapsule;
	ApplyMovementPhysics();

	if (bDoCollisionChecks) PerformCollisionChecks(Velocity);
	
	FVector DesiredMove = Fixed2DToVector(Velocity * FixedDt);
	FighterPawnRef->AddActorWorldOffset(DesiredMove, false);
	
	CollisionCapsule.UpdateCenter(FighterPawnRef->GetFixedLoc());
}

void UFighterMovementComponent::ApplyMovementPhysics()
{
	switch (CurrentMovementMode)
	{
		case EFighterMovementMode::JumpingUp:
			UpdateJumpRise();
			break;
		case EFighterMovementMode::Falling:
			ApplyFallingGravity();
			break;
		case EFighterMovementMode::Grounded:
			Velocity.Z = 0.0f;
		default:
			break;
	}
}

void UFighterMovementComponent::DisplayDebug()
{
	// Display movement mode
	FString ModeName = UEnum::GetValueAsString(CurrentMovementMode);
	ModeName = ModeName.RightChop(FString("EFighterMovementMode::").Len());
	GEngine->AddOnScreenDebugMessage(
		-1,
		0.0f,
		FColor::Cyan,
		FString::Printf(TEXT("MVMT State: %s"), *ModeName)
	);

	// Draw capsules
	//DrawDebugFighterCapsule(FollowCapsule, FColor::Yellow);
	DrawDebugFighterCapsule(CollisionCapsule, FColor::Cyan);
}

void UFighterMovementComponent::SetMovementMode(EFighterMovementMode NewMode)
{
	if (CurrentMovementMode != NewMode) CurrentMovementMode = NewMode;
}

#pragma region JumpHandling

// Called when starting a hop
bool UFighterMovementComponent::DoHop(EHopType HopType)
{
	if (JumpsRemaining <= 0) return false;

	const FHopData* HopData = HopDataMap.Find(HopType);
	if (!HopData || !HopData->Curve) return false;

	CollisionCapsule.LiftBottom();
	CurrentHopData = *HopData;
	HopCurrentFrame = 0;

	Velocity.Z = 0.f;
	Velocity.X = FixedClamp(Velocity.X, -MaxAirSpeed.ToFixed(), MaxAirSpeed);
	SetMovementMode(EFighterMovementMode::JumpingUp);
	JumpsRemaining--;

	return true;
}

void UFighterMovementComponent::UpdateJumpRise()
{
	if (!CurrentHopData.Curve || CurrentHopData.Frames <= 0)
	{
		SetMovementMode(EFighterMovementMode::Falling);
		return;
	}

	const FFixed_32 Alpha =
		FloatToFixed(static_cast<float>(HopCurrentFrame) / static_cast<float>(CurrentHopData.Frames));
	const FFixed_32 HeightRatio =
		FloatToFixed(CurrentHopData.Curve->GetFloatValue(FixedToFloat(Alpha)));

	const FFixed_32 CurrentHeight = HeightRatio * CurrentHopData.Height;

	const FFixed_32 PrevAlpha = FixedClamp(
		FloatToFixed(static_cast<float>(HopCurrentFrame - 1) /
					 static_cast<float>(CurrentHopData.Frames)),
		FloatToFixed(0.f),
		FloatToFixed(1.f)
	);

	const FFixed_32 PrevHeightRatio =
		FloatToFixed(CurrentHopData.Curve->GetFloatValue(FixedToFloat(PrevAlpha)));

	const FFixed_32 PrevHeight = PrevHeightRatio * CurrentHopData.Height;

	const FFixed_32 DeltaHeight = CurrentHeight - PrevHeight;

	Velocity.Z = DeltaHeight / FixedDt;
	HopCurrentFrame++;

	if (HopCurrentFrame >= CurrentHopData.Frames)
	{
		SetMovementMode(EFighterMovementMode::Falling);
	}
}

void UFighterMovementComponent::SetMaxJumpCount(int32 NewMaxJumpCount)
{
	MaxJumpCount = NewMaxJumpCount;
	JumpsRemaining = MaxJumpCount;
}

void UFighterMovementComponent::ResetJumpCount()
{
	JumpsRemaining = MaxJumpCount;
}

#pragma endregion

void UFighterMovementComponent::ApplyGroundFriction()
{
	if (!bCanApplyGroundFriction || Velocity.X == FFixed_32(0.f)) return;

	Velocity.X *= GroundFriction;
	
	const FFixed_32 MinVelocity = FFixed_32(0.01f);
	if (Velocity.X.Abs() < MinVelocity)
	{
		Velocity.X = FFixed_32(0.f);
	}
}

void UFighterMovementComponent::ApplyCustomFriction(FFixed_32 Friction)
{
	if (!bCanApplyGroundFriction || Velocity.X == FFixed_32(0.f)) return;
	
	Velocity.X *= Friction;
	
	const FFixed_32 MinVelocity = FFixed_32(0.01f);
	if (Velocity.X.Abs() < MinVelocity)
	{
		Velocity.X = FFixed_32(0.f);
	}
}

#pragma region Falling/Landing

void UFighterMovementComponent::ApplyAirDrift(FFixed_32 StickX)
{
	if (StickX == FFixed_32(0.f)) // no stick input
	{
		const FFixed_32 Decel = AirFriction.ToFixed() * FixedDt;

		if (Velocity.X > FFixed_32(0.f))
			Velocity.X = FixedMax(FFixed_32(0.f), Velocity.X - Decel);
		else if (Velocity.X < FFixed_32(0.f))
			Velocity.X = FixedMin(FFixed_32(0.f), Velocity.X + Decel);

		return;
	}

	// Target horizontal speed
	const FFixed_32 TargetSpeed = StickX * MaxAirSpeed;

	// Per-frame acceleration amount
	const FFixed_32 MaxDelta = AirAcceleration * FixedDt;

	// Difference to target
	const FFixed_32 SpeedDiff = TargetSpeed - Velocity.X;

	// Move toward target speed without overshooting
	if (SpeedDiff > FFixed_32(0.f))
	{
		Velocity.X += FixedMin(SpeedDiff, MaxDelta);
	}
	else if (SpeedDiff < FFixed_32(0.f))
	{
		Velocity.X += FixedMax(SpeedDiff, -MaxDelta);
	}
}

void UFighterMovementComponent::ApplyFallingGravity()
{
	if (bIsFastFalling) Velocity.Z = TerminalFallVelocity * FastFallMultiplier;
	else
	{
		Velocity.Z -= (FixedDt* Gravity);
		if (Velocity.Z < TerminalFallVelocity) Velocity.Z = TerminalFallVelocity;
	}
}

void UFighterMovementComponent::ProcessLanded()
{
	CollisionCapsule.Reset();
	ResetJumpCount();
	bIsFastFalling = false;
	SetMovementMode(EFighterMovementMode::Grounded);
}

#pragma endregion

FFixedVector2D UFighterMovementComponent::FindFurthestGroundedPosition(int32 InDirection) const
{
	const FFixed_32 StepSize = 0.5f;
	const int32 MaxSteps = 30;                  // Maximum look-ahead steps

	const FFixedVector2D Start = CollisionCapsule.GetBottom();

	FFixedVector2D LastGrounded = Start;

	for (int32 i = 1; i <= MaxSteps; ++i)
	{
		FFixedVector2D TestPos = Start + FFixedVector2D(InDirection * i * StepSize, 0.f);
		FFixedVector2D TraceStart = TestPos + FFixedVector2D(FFixed_32(), 10.f);
		FFixedVector2D TraceEnd   = TestPos - FFixedVector2D(FFixed_32(), 20.f);
		
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(GetOwner());
		
		FFixedHitResult Hit = FixedLineTrace(GetWorld(), TraceStart, TraceEnd);

		if (!Hit.bBlockingHit)
		{
			break; // We've reached the point just past the ledge
		}

		LastGrounded = TestPos;
	}

	// Place actor flush to the ledge
	FFixedVector2D Result = FighterPawnRef->GetFixedLoc();
	Result.X = LastGrounded.X;
	return Result;
}

bool UFighterMovementComponent::WillStayGroundedNextFrame(FFixed_32 HorizontalSpeed, int32 Direction) const
{
	FFixedVector2D CurrentPos = FighterPawnRef->GetFixedLoc();
	FFixed_32 NextX = CurrentPos.X + (Direction * HorizontalSpeed * FixedDt);
	FFixedVector2D PredictPos = FFixedVector2D(NextX, CurrentPos.Z);

	FFixedVector2D TraceStart = PredictPos + FFixedVector2D(0.f, 5.f);
	FFixedVector2D TraceEnd = PredictPos - FFixedVector2D(0.f, 35.f);

	FFixedHitResult Hit = FixedLineTrace(GetWorld(), TraceStart, TraceEnd);
	return Hit.bBlockingHit;
}

void UFighterMovementComponent::SnapToNearestGroundBehindStep(int32 inDirection)
{
	FFixedVector2D SafePosition = FindFurthestGroundedPosition(inDirection);
	Velocity = FFixedVector2D(0.f, 0.f);
	FighterPawnRef->SetFixedLoc(SafePosition);
}

void UFighterMovementComponent::HaltHorizontalVelocity()
{
	Velocity.X = FFixed_32(0.f);
}

void UFighterMovementComponent::HaltVerticalVelocity()
{
	Velocity.Z = FFixed_32(0.f);
}

void UFighterMovementComponent::PreventLedgeFall(bool bPreventFall)
{
	const FFixed_32 CurrentSpeed = Velocity.X.Abs();
	int32 Direction = Velocity.X.Sign();

	if (!WillStayGroundedNextFrame(CurrentSpeed, Direction))
	{
		if (bPreventFall)
		{
			HaltHorizontalVelocity();
			SnapToNearestGroundBehindStep(Direction);
			SetMovementMode(EFighterMovementMode::Grounded);
		}
		else
		{
			SetMovementMode(EFighterMovementMode::Falling);
			JumpsRemaining--;
		}
	}
}

void UFighterMovementComponent::PreventLedgeFall(bool bPreventFall, FFixedVector2D& InVelocity)
{
	const FFixed_32 CurrentSpeed = InVelocity.X.Abs();
	int32 Direction = InVelocity.X.Sign();

	if (!WillStayGroundedNextFrame(CurrentSpeed, Direction))
	{
		if (bPreventFall)
		{
			InVelocity.X = FFixed_32(0.f);
			SnapToNearestGroundBehindStep(Direction);
			SetMovementMode(EFighterMovementMode::Grounded);
		}
		else
		{
			SetMovementMode(EFighterMovementMode::Falling);
			JumpsRemaining--;
		}
	}
}

bool UFighterMovementComponent::IsStandingOnFacingLedge() const
{
	const int32 FacingDir = FighterPawnRef->IsFacingRight() ? 1 : -1;
	FFixed_32 ProbeSpeed = 300.f;
	
	return !WillStayGroundedNextFrame(ProbeSpeed, FacingDir);
}

void UFighterMovementComponent::StopMovementCompletely(bool bStopCollision)
{
	Velocity = FFixedVector2D(0.f, 0.f);
	SetMovementMode(EFighterMovementMode::None);
	if (bStopCollision) bDoCollisionChecks = false;
}

void UFighterMovementComponent::DrawDebugFighterCapsule(const FFighterCapsule& Capsule, const FColor& Color)
{
	DrawDebugCapsule(
		GetWorld(),
		Fixed2DToVector(Capsule.GetCenter()),
		FixedToFloat(Capsule.HalfHeight),
		FixedToFloat(Capsule.Radius),
		FQuat::Identity,
		Color,
		false,
		0.0f,
		0,
		2.0f
	);
}

void UFighterMovementComponent::PerformCollisionChecks(FFixedVector2D &InVelocity)
{
	if (InVelocity.Z < FFixed_32(0.f))
	{
		if (PerformGroundCollisionCheck(InVelocity).bBlockingHit)
		{
			ProcessLanded();
		}
	}
	else if (InVelocity.Z > FFixed_32(0.f)) PerformCeilingCollisionCheck(InVelocity);

	PerformWallCollisionCheck(InVelocity);
}

FFixedHitResult UFighterMovementComponent::PerformWallCollisionCheck(FFixedVector2D& InVelocity)
{
	FFixedHitResult OutHit;
	
	const FFixedVector2D Start = CollisionCapsule.GetCenter() + FFixedVector2D(0.f, InVelocity.Z * FixedDt);
	const FFixedVector2D TraceDelta(InVelocity.X * FixedDt, 0.f);
	
	if (InVelocity.X != FFixed_32(0.f))
	{
		const FFixedVector2D End = Start + TraceDelta;
		OutHit = FixedSweepCapsule(GetWorld(), Start, End, CollisionCapsule.Radius, CollisionCapsule.HalfHeight);
		{
			if (OutHit.Normal.X.Abs() > 0.5f)
			{
				int32 Dir = InVelocity.X.Sign() * -1;
				FFixedVector2D ActorLoc = FighterPawnRef->GetFixedLoc();
				ActorLoc.X = OutHit.ImpactPoint.X + (Dir * CollisionCapsule.GetRadiusWithBuffer());
				FighterPawnRef->SetFixedLoc(ActorLoc);
				InVelocity.X = FFixed_32(0.f);
				CollisionCapsule.UpdateCenter(FighterPawnRef->GetFixedLoc());
				return OutHit;
			}
		}
	}
	else // no horizontal velocity, but still check for walls
	{
		const FFixed_32 SmallCheckDist = 2.f;

		FFixedVector2D TempRight(SmallCheckDist, InVelocity.Z);
		FFixedVector2D TempLeft (-SmallCheckDist, InVelocity.Z);

		FFixedHitResult HitRight = PerformWallCollisionCheck(TempRight);
		FFixedHitResult HitLeft  = PerformWallCollisionCheck(TempLeft);

		if (HitRight.bBlockingHit) return HitRight;
		if (HitLeft.bBlockingHit) return HitLeft;
	}

	return OutHit;
}

FFixedHitResult UFighterMovementComponent::PerformGroundCollisionCheck(FFixedVector2D &InVelocity)
{
	FFixedHitResult OutHit;
	if (InVelocity.Z >= FFixed_32(0.f)) return OutHit;
	
	FFixedVector2D Start = CollisionCapsule.GetBottom();
	Start.X += InVelocity.X * FixedDt;
    
	FFixed_32 TraceDistanceZ = InVelocity.Z * FixedDt;
    
	FFixedVector2D End = Start + FFixedVector2D(0.f, TraceDistanceZ);

	OutHit = FixedLineTrace(GetWorld(), Start, End);
    
	if (OutHit.bBlockingHit)
	{
		FFixedVector2D NewLocation = OutHit.ImpactPoint;
		NewLocation.Z += CollisionCapsule.bufferlayer;
		FighterPawnRef->SetFixedLoc(NewLocation);
		HaltVerticalVelocity();
		CollisionCapsule.UpdateCenter(NewLocation);
	}
	
	return OutHit;
}

FFixedHitResult UFighterMovementComponent::PerformCeilingCollisionCheck(FFixedVector2D &InVelocity)
{
	FFixedHitResult OutHit;
	if (InVelocity.Z <= 0.f) return OutHit;
	FFixedVector2D Start = CollisionCapsule.GetCenter() + FFixedVector2D(InVelocity.X * FixedDt, CollisionCapsule.HalfHeight);
	FFixedVector2D End = Start + FFixedVector2D(0.f, InVelocity.Z * FixedDt);

	OutHit = FixedLineTrace(GetWorld(), Start, End);
    
	if (OutHit.bBlockingHit)
	{
		const FFixed_32 CapsuleTopOffset = (CollisionCapsule.defaultHalfHeight.ToFixed() * 2) + CollisionCapsule.bufferlayer.ToFixed();
		const FFixedVector2D NewLocation = OutHit.ImpactPoint - FFixedVector2D(0.f, CapsuleTopOffset);
		FighterPawnRef->SetFixedLoc(NewLocation);
		HaltVerticalVelocity();
		CollisionCapsule.UpdateCenter(NewLocation);
	}
	return OutHit;
}

void UFighterMovementComponent::ManualDisplacement(FFixedVector2D Movement, bool bPreventLedgeFall)
{
	FFixedVector2D TempVelocity = Movement / FixedDt;
	PerformCollisionChecks(TempVelocity);
	PreventLedgeFall(bPreventLedgeFall, TempVelocity);
	FVector DesiredMove = Fixed2DToVector(TempVelocity * FixedDt);
	FighterPawnRef->AddActorWorldOffset(DesiredMove, false);
	
	CollisionCapsule.UpdateCenter(FighterPawnRef->GetFixedLoc());
}