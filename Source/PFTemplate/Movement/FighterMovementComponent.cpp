#include "FighterMovementComponent.h"
#include "GameFramework/Actor.h"
#include "DrawDebugHelpers.h"
#include "FighterPawn.h"
#include "FixedCollision.h"

UFighterMovementComponent::UFighterMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PhysicsSnapshot.JumpsRemaining = MaxJumpCount;
}

void UFighterMovementComponent::InitFMC(AFighterPawn* InFighterPawn)
{
	FighterPawnRef = InFighterPawn;
	CollisionCapsule.Reset();
	CollisionCapsule.UpdateCenter(FighterPawnRef->GetFixedLoc());
}

void UFighterMovementComponent::TickFMC()
{
	if (!FighterPawnRef && PhysicsSnapshot.bStopMovementUpdates) return;
	
	FollowCapsule = CollisionCapsule;
	ApplyMovementPhysics();

	if (PhysicsSnapshot.bDoCollisionChecks) PerformCollisionChecks(PhysicsSnapshot.Velocity);
	
	FVector DesiredMove = Fixed2DToVector(GetVelocity() * FixedDt);
	FighterPawnRef->AddActorWorldOffset(DesiredMove, false);
	
	CollisionCapsule.UpdateCenter(FighterPawnRef->GetFixedLoc());
}

void UFighterMovementComponent::ApplyMovementPhysics()
{
	switch (PhysicsSnapshot.CurrentMovementMode)
	{
		case EFighterMovementMode::JumpingUp:
			UpdateJumpRise();
			break;
		case EFighterMovementMode::Falling:
			ApplyFallingGravity();
			break;
		case EFighterMovementMode::Grounded:
			PhysicsSnapshot.Velocity.Z = FFixed_32(0);
		default:
			break;
	}
}

void UFighterMovementComponent::DisplayDebug()
{
	// Display movement mode
	FString ModeName = UEnum::GetValueAsString(PhysicsSnapshot.CurrentMovementMode);
	ModeName = ModeName.RightChop(FString("EFighterMovementMode::").Len());
	GEngine->AddOnScreenDebugMessage(
		-1,
		0.0f,
		FColor::Cyan,
		FString::Printf(TEXT("MVMT State: %s"), *ModeName)
	);
}

void UFighterMovementComponent::SetMovementMode(EFighterMovementMode NewMode)
{
	switch (NewMode)
	{
		case EFighterMovementMode::KeepCurrent: return;
		case EFighterMovementMode::AutoResolve:
			{
				TestIsGrounded();
				if (!IsGrounded()) SetMovementMode(EFighterMovementMode::Falling);
				return;
			}
		case EFighterMovementMode::Falling:
			{
				PhysicsSnapshot.bOnPlatform = false;
			}
	case EFighterMovementMode::Custom: PhysicsSnapshot.bIsFastFalling = false;
		default: break;
	}
	SetMovementMode(NewMode);
}

#pragma region Jump Handling


bool UFighterMovementComponent::StartJump(EJumpType JumpType)
{
	if (PhysicsSnapshot.JumpsRemaining <= 0) return false;

	const FJumpData* JumpData = JumpDataMap.Find(JumpType);
	if (!JumpData || JumpData->FramesToApex <= 0) return false;

	//CollisionCapsule.LiftBottom();

	FFixed_32 time = JumpData->FramesToApex * FixedDt;
	PhysicsSnapshot.Velocity.Z = FFixed_32(2.f) * JumpData->JumpHeight / time;
	RisingGravity = FFixed_32(2.f) * JumpData->JumpHeight/ (time * time);

	SetMovementMode(EFighterMovementMode::JumpingUp);
	PhysicsSnapshot.JumpsRemaining--;
	PhysicsSnapshot.bOnPlatform = false;
	PhysicsSnapshot.bIsFastFalling = false;
	return true;
}

void UFighterMovementComponent::UpdateJumpRise()
{
	PhysicsSnapshot.Velocity.Z -= RisingGravity * FixedDt;

	if (PhysicsSnapshot.Velocity.Z <= FFixed_32(0.0f)) SetMovementMode(EFighterMovementMode::Falling);
}

void UFighterMovementComponent::ResetJumpCount()
{
	PhysicsSnapshot.JumpsRemaining = MaxJumpCount;
}

#pragma endregion

void UFighterMovementComponent::ApplyGroundFriction()
{
	if (PhysicsSnapshot.Velocity.X == FFixed_32(0.f)) return;
	
	FFixed_32 CurrentTraction = GroundTraction;
	if (PhysicsSnapshot.Velocity.X.Abs() > FFixed_32(WalkSpeed) * FFixed_32(1.3f)) CurrentTraction *= FFixed_32(2.f);
	
	FFixed_32 DecelAmount = CurrentTraction;
	
	FFixed_32 Direction = PhysicsSnapshot.Velocity.X.Sign();
	FFixed_32 PrevVelocityX = PhysicsSnapshot.Velocity.X;
    
	PhysicsSnapshot.Velocity.X -= DecelAmount * Direction;
	
	if ((PrevVelocityX > 0 && PhysicsSnapshot.Velocity.X <= 0) || (PrevVelocityX < 0 && PhysicsSnapshot.Velocity.X >= 0))
	{
		PhysicsSnapshot.Velocity.X = FFixed_32(0.f);
	}
}

#pragma region Falling/Landing

void UFighterMovementComponent::ApplyAirDrift(FFixed_32 StickX)
{
	if (StickX == FFixed_32(0.f)) // no stick input
	{
		const FFixed_32 Decel = FFixed_32(AirFriction) * FixedDt;

		if (PhysicsSnapshot.Velocity.X > FFixed_32(0.f))
			PhysicsSnapshot.Velocity.X = FixedMax(FFixed_32(0.f), PhysicsSnapshot.Velocity.X - Decel);
		else if (PhysicsSnapshot.Velocity.X < FFixed_32(0.f))
			PhysicsSnapshot.Velocity.X = FixedMin(FFixed_32(0.f), PhysicsSnapshot.Velocity.X + Decel);

		return;
	}

	// Target horizontal speed
	const FFixed_32 TargetSpeed = StickX * MaxAirSpeed;

	// Per-frame acceleration amount
	const FFixed_32 MaxDelta = AirAcceleration * FixedDt;

	// Difference to target
	const FFixed_32 SpeedDiff = TargetSpeed - PhysicsSnapshot.Velocity.X;

	// Move toward target speed without overshooting
	if (SpeedDiff > FFixed_32(0.f))
	{
		PhysicsSnapshot.Velocity.X += FixedMin(SpeedDiff, MaxDelta);
	}
	else if (SpeedDiff < FFixed_32(0.f))
	{
		PhysicsSnapshot.Velocity.X += FixedMax(SpeedDiff, -MaxDelta);
	}
}

void UFighterMovementComponent::ApplyFallingGravity()
{
	if (PhysicsSnapshot.bIsFastFalling) PhysicsSnapshot.Velocity.Z = TerminalFallVelocity * FastFallMultiplier;
	else
	{
		PhysicsSnapshot.Velocity.Z -= (FixedDt* Gravity);
		if (PhysicsSnapshot.Velocity.Z < TerminalFallVelocity) PhysicsSnapshot.Velocity.Z = TerminalFallVelocity;
	}
}

void UFighterMovementComponent::ProcessLanded()
{
	CollisionCapsule.Reset();
	ResetJumpCount();
	PhysicsSnapshot.bIsFastFalling = false;
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
	PhysicsSnapshot.Velocity = FFixedVector2D(0.f, 0.f);
	FighterPawnRef->SetFixedLoc(SafePosition);
}

void UFighterMovementComponent::HaltHorizontalVelocity()
{
	PhysicsSnapshot.Velocity.X = FFixed_32(0.f);
}

void UFighterMovementComponent::HaltVerticalVelocity()
{
	PhysicsSnapshot.Velocity.Z = FFixed_32(0.f);
}

void UFighterMovementComponent::PreventLedgeFall(FFixedVector2D& InVelocity, bool bPreventFall)
{
	if (!IsGrounded()) return;
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
			PhysicsSnapshot.Velocity.X = FixedClamp(PhysicsSnapshot.Velocity.X, -MaxAirSpeed, MaxAirSpeed);
			PhysicsSnapshot.JumpsRemaining--;
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
	PhysicsSnapshot.Velocity = FFixedVector2D(0.f, 0.f);
	SetMovementMode(EFighterMovementMode::Custom);
	if (bStopCollision) PhysicsSnapshot.bDoCollisionChecks = false;
}

void UFighterMovementComponent::PerformCollisionChecks(FFixedVector2D &InVelocity)
{
	if (InVelocity.Z < FFixed_32(0.f))
	{
		if (PerformGroundCollisionCheck(InVelocity).bBlockingHit) ProcessLanded();
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
				if (OutHit.HitActor && OutHit.HitActor->ActorHasTag(FName("Platform"))) return FFixedHitResult();
				int32 Dir = OutHit.Normal.X.Sign();
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

	return FFixedHitResult();
}

FFixedHitResult UFighterMovementComponent::PerformGroundCollisionCheck(FFixedVector2D &InVelocity)
{
	FFixedHitResult OutHit;
	if (InVelocity.Z >= FFixed_32(0.f)) return FFixedHitResult();
	
	FFixedVector2D Start = CollisionCapsule.GetBottom();
	Start.X += InVelocity.X * FixedDt;
    
	FFixed_32 TraceDistanceZ = InVelocity.Z * FixedDt;
    
	FFixedVector2D End = Start + FFixedVector2D(0.f, TraceDistanceZ);
	Start.Z += FFixed_32(1.f);

	OutHit = FixedLineTrace(GetWorld(), Start, End);
    
	if (OutHit.bBlockingHit && !IsGrounded())
	{
		PhysicsSnapshot.bOnPlatform = (OutHit.HitActor && OutHit.HitActor->ActorHasTag(FName("Platform")));
		if (PhysicsSnapshot.bIgnorePlatform && PhysicsSnapshot.bOnPlatform || (PhysicsSnapshot.bIsFastFalling && PhysicsSnapshot.bOnPlatform))
		{
			PhysicsSnapshot.bOnPlatform = false;
			return FFixedHitResult();
		}
		FFixedVector2D NewLocation = OutHit.ImpactPoint;
		//Add buffer layer here
		FighterPawnRef->SetFixedLoc(NewLocation);
		HaltVerticalVelocity();
		CollisionCapsule.UpdateCenter(NewLocation);
	}
	
	return OutHit;
}

FFixedHitResult UFighterMovementComponent::PerformCeilingCollisionCheck(FFixedVector2D &InVelocity)
{
	FFixedHitResult OutHit;
	if (InVelocity.Z <= 0.f) return FFixedHitResult();
	FFixedVector2D Start = CollisionCapsule.GetCenter() + FFixedVector2D(InVelocity.X * FixedDt, CollisionCapsule.HalfHeight);
	FFixedVector2D End = Start + FFixedVector2D(0.f, InVelocity.Z * FixedDt);

	OutHit = FixedLineTrace(GetWorld(), Start, End);
    
	if (OutHit.bBlockingHit)
	{
		if (OutHit.HitActor && OutHit.HitActor->ActorHasTag(FName("Platform"))) return FFixedHitResult();
		//add buffer layer here
		const FFixedVector2D NewLocation; //needs update here
		FighterPawnRef->SetFixedLoc(NewLocation);
		HaltVerticalVelocity();
		CollisionCapsule.UpdateCenter(NewLocation);
	}
	return OutHit;
}

void UFighterMovementComponent::ManualDisplacement(FFixedVector2D Movement /*not velocity*/, bool bPreventLedgeFall)
{
	FFixedVector2D TempVelocity = Movement / FixedDt;
	PerformCollisionChecks(TempVelocity);
	PreventLedgeFall(TempVelocity, bPreventLedgeFall);
	FVector DesiredMove = Fixed2DToVector(TempVelocity * FixedDt);
	LogFixedVector2D("Actual movement", TempVelocity * FixedDt);
	FighterPawnRef->AddActorWorldOffset(DesiredMove, false);
	
	CollisionCapsule.UpdateCenter(FighterPawnRef->GetFixedLoc());
}

void UFighterMovementComponent::TestIsGrounded()
{
	FFixedVector2D TestVelocity(PhysicsSnapshot.Velocity.X, 120.f);
	PerformGroundCollisionCheck(TestVelocity).bBlockingHit;
}