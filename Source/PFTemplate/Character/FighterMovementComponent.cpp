#include "FighterMovementComponent.h"
#include "GameFramework/Actor.h"
#include "DrawDebugHelpers.h"
#include "FighterPawn.h"

UFighterMovementComponent::UFighterMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UFighterMovementComponent::InitFMC(AFighterPawn* InFighterPawn)
{
	FighterPawnRef = InFighterPawn;
	CollisionCapsule.Reset();
	CollisionCapsule.UpdateCenter(GetOwner()->GetActorLocation());
}

void UFighterMovementComponent::TickFMC()
{
	if (!FighterPawnRef) return;

	ApplyMovementPhysics();

	if (bDoCollisionChecks) PerformCollisionChecks();
	
	FVector DesiredMove = Fixed2DToVector(Velocity * FixedDt);
	FighterPawnRef->AddActorWorldOffset(DesiredMove, false);
	
	UpdateCapsule();
}

void UFighterMovementComponent::UpdateCapsule()
{
	FollowCapsule = CollisionCapsule;
	CollisionCapsule.UpdateCenter(GetOwner()->GetActorLocation());
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
	DrawDebugFighterCapsule(FollowCapsule, FColor::Yellow);
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
	Velocity.X = FMath::Clamp(Velocity.X, -MaxAirSpeed, MaxAirSpeed);
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

	const FIXED_32 Alpha =
		FloatToFixed(static_cast<float>(HopCurrentFrame) / static_cast<float>(CurrentHopData.Frames));
	const FIXED_32 HeightRatio =
		FloatToFixed(CurrentHopData.Curve->GetFloatValue(FixedToFloat(Alpha)));

	const FIXED_32 CurrentHeight = HeightRatio * CurrentHopData.Height;

	const FIXED_32 PrevAlpha =
		FloatToFixed(static_cast<float>(HopCurrentFrame - 1) / static_cast<float>(CurrentHopData.Frames))
		.Clamp(FloatToFixed(0.f), FloatToFixed(1.f));

	const FIXED_32 PrevHeightRatio =
		FloatToFixed(CurrentHopData.Curve->GetFloatValue(FixedToFloat(PrevAlpha)));

	const FIXED_32 PrevHeight = PrevHeightRatio * CurrentHopData.Height;

	const FIXED_32 DeltaHeight = CurrentHeight - PrevHeight;

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
	if (!bCanApplyGroundFriction) return;

	Velocity.X *= GroundFriction;
	
	const FIXED_32 MinVelocity = FIXED_32(0.01f);
	if (Velocity.X.Abs() < MinVelocity)
	{
		Velocity.X = FIXED_32(0.f);
	}
}

void UFighterMovementComponent::ApplyCustomFriction(FIXED_32 Friction)
{
	Velocity.X *= Friction;
	
	const FIXED_32 MinVelocity = FIXED_32(0.01f);
	if (Velocity.X.Abs() < MinVelocity)
	{
		Velocity.X = FIXED_32(0.f);
	}
}

#pragma region Falling/Landing

void UFighterMovementComponent::ApplyAirDrift(float StickX)
{
	constexpr float FrameDuration = 1.f / 60.f;

	FVector CurrentVelocity = GetVelocity();

	if (FMath::IsNearlyZero(StickX, 0.01f))
	{
		float Decel = AirFriction * FixedFrameTime;
    
		// apply deceleration toward zero
		if (CurrentVelocity.X > 0.f)
			CurrentVelocity.X = FMath::Max(0.f, CurrentVelocity.X - Decel);
		else if (CurrentVelocity.X < 0.f)
			CurrentVelocity.X = FMath::Min(0.f, CurrentVelocity.X + Decel);

		SetVelocity(CurrentVelocity);
		return;
	}

	// Compute target speed
	const float TargetSpeed = StickX * MaxAirSpeed;

	// Apply acceleration toward target speed
	if (!FMath::IsNearlyEqual(CurrentVelocity.X, TargetSpeed, 0.1f))
	{
		const float DeltaSpeed = FMath::Sign(TargetSpeed - CurrentVelocity.X) * AirAcceleration * FrameDuration;
		CurrentVelocity.X += DeltaSpeed;

		// Clamp to target
		if ((TargetSpeed - CurrentVelocity.X) * DeltaSpeed < 0.f)
		{
			CurrentVelocity.X = TargetSpeed;
		}
	}

	SetVelocity(CurrentVelocity);
}

void UFighterMovementComponent::ApplyFallingGravity()
{
	if (bIsFastFalling) Velocity.Z = TerminalFallVelocity * FastFallMultiplier;
	else
	{
		Velocity.Z -= Gravity * FixedFrameTime;
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

FVector UFighterMovementComponent::FindFurthestGroundedPosition(float InDirection) const
{
	constexpr float FixedDeltaTime = 1.f / 60.f;
	const float StepSize = 0.5f;               // Tune this to go closer or coarser
	const int MaxSteps = 30;                  // Maximum look-ahead steps

	const FVector Start = CollisionCapsule.GetBottom();
	const float Direction = InDirection;

	FVector LastGrounded = Start;

	for (int i = 1; i <= MaxSteps; ++i)
	{
		FVector TestPos = Start + FVector(Direction * i * StepSize, 0.f, 0.f);
		FVector TraceStart = TestPos + FVector(0.f, 0.f, 10.f);
		FVector TraceEnd   = TestPos - FVector(0.f, 0.f, 20.f);

		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(GetOwner());

		bool bGrounded = GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, Params);

		if (!bGrounded)
		{
			break; // We've reached the point just past the ledge
		}

		LastGrounded = TestPos;
	}

	// Place actor flush to the ledge
	FVector Result = GetOwner()->GetActorLocation();
	Result.X = LastGrounded.X;
	return Result;
}

bool UFighterMovementComponent::WillStayGroundedNextFrame(float HorizontalSpeed, float Direction) const
{
	FVector CurrentPos = GetOwner()->GetActorLocation();
	float NextX = CurrentPos.X + (Direction * HorizontalSpeed * FixedFrameTime);
	FVector PredictPos = FVector(NextX, CurrentPos.Y, CurrentPos.Z);

	FVector TraceStart = PredictPos + FVector(0.f, 0.f, 5.f);
	FVector TraceEnd = PredictPos - FVector(0.f, 0.f, 35.f);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	return GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, Params);
}

void UFighterMovementComponent::SnapToNearestGroundBehindStep(float Direction)
{
	FVector SafePosition = FindFurthestGroundedPosition();
	SetVelocity(FVector::ZeroVector);
	GetOwner()->SetActorLocation(SafePosition);
}

void UFighterMovementComponent::HaltHorizontalVelocity()
{
	FVector NewVelocity = Velocity;
	NewVelocity.X = 0.f;
	SetVelocity(NewVelocity);
}

void UFighterMovementComponent::HaltVerticalVelocity()
{
	FVector NewVelocity = Velocity;
	NewVelocity.Z = 0.f;
	SetVelocity(NewVelocity);
}

void UFighterMovementComponent::HandleLedgeOrFall(bool bPreventFall)
{
	const float CurrentSpeed = FMath::Abs(Velocity.X);
	const float Direction = (Velocity.X > 0.f) ? 1.f : -1.f;

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

bool UFighterMovementComponent::IsStandingOnFacingLedge() const
{
	const float FacingDir = FighterPawnRef->IsFacingRight() ? 1.f : -1.f;
	constexpr float ProbeSpeed = 300.f;
	
	return !WillStayGroundedNextFrame(ProbeSpeed, FacingDir);
}

void UFighterMovementComponent::StopMovementCompletely(bool bStopCollision)
{
	SetVelocity(FVector::ZeroVector);
	SetMovementMode(EFighterMovementMode::None);
	if (bStopCollision) bDoCollisionChecks = false;
}

void UFighterMovementComponent::DrawDebugFighterCapsule(const FFighterCapsule& Capsule, const FColor& Color)
{
	DrawDebugCapsule(
		GetWorld(),
		Capsule.GetCenter(),
		Capsule.HalfHeight,
		Capsule.Radius,
		FQuat::Identity,
		Color,
		false,
		0.0f,
		0,
		2.0f
	);
}

void UFighterMovementComponent::PerformCollisionChecks()
{
	FHitResult WallHit, GroundHit, CeilingHit;
	bool bHitWall = PerformWallCollisionCheck(Velocity, WallHit);
	
	if (CurrentMovementMode == EFighterMovementMode::Falling)
	{
		if (PerformGroundCollisionCheck(Velocity, GroundHit, bHitWall))
		{
			ProcessLanded();
			return;
		}
	}
	
	PerformCeilingCollisionCheck(Velocity, CeilingHit, bHitWall);
}

bool UFighterMovementComponent::PerformWallCollisionCheck(FVector& InVelocity, FHitResult& OutHit)
{
	const FVector Start = CollisionCapsule.GetCenter();
	const float SmallCheckDist = CollisionCapsule.Radius * 0.2f;
	const FVector TraceDelta = InVelocity * FixedFrameTime;

	// Horizontal velocity case
	if (InVelocity.X != 0.f)
	{
		const FVector End = Start + TraceDelta;
		if (GetWorld()->SweepSingleByChannel(
			OutHit,
			Start,
			End,
			FQuat::Identity,
			ECC_WorldStatic,
			GetCapsule()
		))
		{
			const FVector Normal = OutHit.ImpactNormal;
			if (FMath::Abs(Normal.X) > 0.5f)
			{
				FVector ActorLoc = GetOwner()->GetActorLocation();
				ActorLoc.X = OutHit.ImpactPoint.X + (Normal.X * (CollisionCapsule.Radius + CollisionCapsule.bufferlayer));
				GetOwner()->SetActorLocation(ActorLoc);
				InVelocity.X = 0.f;
				CollisionCapsule.UpdateCenter(ActorLoc);
				return true;
			}
		}
	}
	else // No horizontal velocity: check small forward/back traces
	{
		FHitResult TempHit;
		const FVector ForwardStart = Start + FVector(SmallCheckDist, 0, 0);
		const FVector BackwardStart = Start - FVector(SmallCheckDist, 0, 0);

		if (GetWorld()->SweepSingleByChannel(
			TempHit,
			Start,
			ForwardStart,
			FQuat::Identity,
			ECC_WorldStatic,
			GetCapsule()
		) || GetWorld()->SweepSingleByChannel(
			TempHit,
			Start,
			BackwardStart,
			FQuat::Identity,
			ECC_WorldStatic,
			GetCapsule()
		))
		{
			if (FMath::Abs(TempHit.ImpactNormal.X) > 0.5f)
			{
				FVector ActorLoc = GetOwner()->GetActorLocation();
				ActorLoc.X = TempHit.ImpactPoint.X + (TempHit.ImpactNormal.X * (CollisionCapsule.Radius + CollisionCapsule.bufferlayer));
				GetOwner()->SetActorLocation(ActorLoc);
				CollisionCapsule.UpdateCenter(ActorLoc);
				OutHit = TempHit;
				return true;
			}
		}
	}

	return false;
}

bool UFighterMovementComponent::PerformGroundCollisionCheck(FVector &InVelocity, FHitResult &OutHit, bool bHitWall)
{
	if (InVelocity.Z >= 0.f) return false;
	FVector Start = CollisionCapsule.GetBottom();
	Start.X += bHitWall ? 0.f : InVelocity.X * FixedFrameTime;
    
	float TraceDistance = InVelocity.Z * 1/60;
    
	FVector End = Start + FVector(0, 0, TraceDistance);
    
	bool bHit = GetWorld()->LineTraceSingleByChannel(
	   OutHit,
	   Start,
	   End,
	   ECC_WorldStatic
	);
    
	if (bHit)
	{
		FVector NewLocation = OutHit.Location;
		NewLocation.X -= InVelocity.X * 1/60;
		NewLocation.Z += CollisionCapsule.bufferlayer;
		GetOwner()->SetActorLocation(NewLocation);
		Velocity.Z = 0.f;
		CollisionCapsule.UpdateCenter(NewLocation);
		return true;
	}
	return false;
}

bool UFighterMovementComponent::PerformCeilingCollisionCheck(FVector &InVelocity, FHitResult &OutHit, bool bHitWall)
{
	if (InVelocity.Z <= 0.f) return false;
	FVector Start = CollisionCapsule.GetCenter() + FVector(bHitWall ? 0 : InVelocity.X * FixedFrameTime, 0.f, CollisionCapsule.HalfHeight);
    
	float TraceDistance = InVelocity.Z * 1/60;
    
	FVector End = Start + FVector(0, 0, TraceDistance);
    
	bool bHit = GetWorld()->LineTraceSingleByChannel(
	   OutHit,
	   Start,
	   End,
	   ECC_WorldStatic
	);
    
	if (bHit)
	{
		const float CapsuleTopOffset = CollisionCapsule.defaultHalfHeight * 2 + CollisionCapsule.bufferlayer;
		const FVector NewLocation = OutHit.ImpactPoint - FVector(0, 0, CapsuleTopOffset);
		GetOwner()->SetActorLocation(NewLocation);
		InVelocity.Z = 0.f;
		CollisionCapsule.UpdateCenter(NewLocation);
		return true;
	}
	return false;
}

FCollisionShape UFighterMovementComponent::GetCapsule() const
{
	return
	FCollisionShape::MakeCapsule
		(
			CollisionCapsule.Radius,
			CollisionCapsule.HalfHeight
		);
}