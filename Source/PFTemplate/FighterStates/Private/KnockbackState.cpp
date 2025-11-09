#include "KnockbackState.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.h"

void UKnockbackState::InitState(AFighterPawn* InFighterPawn, UFighterMovementComponent* InMoveComp, UFighterStateMachine* InStateMachine)
{
	Super::InitState(InFighterPawn, InMoveComp, InStateMachine);
	weight = InFighterPawn->Weight;
	gravity = InMoveComp->Gravity;
}

void UKnockbackState::InitKnockback(float h, float d, float KBG, float BKB, float a, float x)
{
	const float KnockbackValue = (((h/10 + (h * d/20)) * (200.f / (weight + 100.f) * 1.4f) + 18.f) * KBG) + BKB;

	LaunchSpeed = (KnockbackValue * 0.3);
	if (x < 0) LaunchAngle = 180.f - a;
	else LaunchAngle = a;
	Duration = FMath::Floor(KnockbackValue * 0.4);
	CheckTumble();
}

void UKnockbackState::OnEnter()
{
	MoveComp->bDoCollisionChecks = false;
	bSliding = false;
	
	FDamageInfo Dam = FighterPawnRef->StoredDamageInfo;
	InitKnockback(
		FighterPawnRef->Health/10,
		Dam.Damage/10,
		Dam.KnockbackGrowth,
		Dam.BaseKnockback,
		Dam.Angle,
		FighterPawnRef->GetFacingDirection() * -1.f
		);

	FighterPawnRef->SetCurrentAction("Knockback", Duration);
}

void UKnockbackState::Tick()
{
	if (StateMachine->FramesInState == Duration)
	{
		StateMachine->TryChangeState(MoveComp->IsGrounded() ? "Idle" : bTumble ? "Tumble" : "Falling");
		return;
	}
	FVector Velocity;
	CalcKBPosUpdate(Velocity);
	const FVector SavedVelocity = Velocity;

	FHitResult HitResult = DoCollisionCheck(Velocity);
	const bool bHit       = HitResult.bBlockingHit;
	const bool bGroundHit = HitResult.Normal.Z >= 0.7f;
	const bool bIsMeteor  = IsMeteor();
	
	// Wall Bounce
	if (bTumble && bHit && !bGroundHit)
	{
		LaunchSpeed *= 0.9f;
		Velocity *= 0.9f;
		Velocity = CalcReflect(SavedVelocity, HitResult.Normal);
	}

	// Ground Bounce
	else if (bTumble && bHit && bGroundHit && bIsMeteor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Before Bounce | Velocity: %s"), *SavedVelocity.ToString());
		
		Velocity = CalcReflect(SavedVelocity, HitResult.Normal);
		LaunchSpeed *= 0.4f;
		Velocity *= 0.4f;

		UE_LOG(LogTemp, Warning, TEXT("After Bounce  | Velocity: %s"), *Velocity.ToString());
	}
	
	// Begin Slide
	else if (bHit && bGroundHit && !bSliding && !bTumble)
	{
		UE_LOG(LogTemp, Warning, TEXT("EnterSlide"));
		MoveComp->SetMovementMode(EFighterMovementMode::Grounded);
		LaunchAngle = FMath::RadiansToDegrees(FMath::Atan2(Velocity.Z, Velocity.X));
		bSliding = true;
		FighterPawnRef->SetCurrentAction("Sliding");

		Velocity *= 0.95f;
		LaunchSpeed *= 0.95f;
	}

	// Slide Physics
	if (bSliding)
	{
		Velocity.X *= MoveComp->GroundFriction;
		Velocity.X = FMath::Clamp(Velocity.X, -498.f, 498.f); // 8.3 * 60
	}

	CompleteKBPosUpdate(Velocity);
}

void UKnockbackState::CalcKBPosUpdate(FVector &InVelocity)
{
	if (LaunchSpeed <= 0.f) LaunchSpeed = 0;
	
	const float Radians = FMath::DegreesToRadians(LaunchAngle);
	FVector PosUpdate = FVector
	(
		FMath::Cos(Radians) * LaunchSpeed,
		0.f,
		FMath::Sin(Radians) * LaunchSpeed
	);
	
	InVelocity = PosUpdate * 60.f; //fixed 60 fps
	if (InVelocity.Z > MoveComp->TerminalFallVelocity) InVelocity.Z -= gravity * FixedFrameTime;
	LaunchAngle = FMath::RadiansToDegrees(FMath::Atan2(InVelocity.Z, InVelocity.X));
}

FVector UKnockbackState::CalcReflect(const FVector& InVel, const FVector& Normal)
{
	FVector NewAngle = InVel - 2.f * FVector::DotProduct(InVel, Normal) * Normal;
	LaunchAngle = FMath::RadiansToDegrees(FMath::Atan2(NewAngle.Z, NewAngle.X));
	CheckTumble();
	return NewAngle;
}

FHitResult UKnockbackState::DoCollisionCheck(FVector &InVelocity)
{
	FHitResult CeilingHit, GroundHit, WallHit;

	// zeroes out velocity in direction of collision check
	bool bWall = MoveComp->PerformWallCollisionCheck(InVelocity, WallHit);
	bool bGround = MoveComp->PerformGroundCollisionCheck(InVelocity, GroundHit, bWall);
	bool bCeiling = MoveComp->PerformCeilingCollisionCheck(InVelocity, CeilingHit, bWall);

	if (bGround) return GroundHit;
	if (bWall) return WallHit;
	if (bCeiling) return CeilingHit;
	return FHitResult();
}

void UKnockbackState::CompleteKBPosUpdate(FVector& InVelocity)
{
	MoveComp->SetVelocity(InVelocity);
	LaunchSpeed -= 0.51f;
	ShowDebugKB(true, InVelocity);
}


void UKnockbackState::ShowDebugKB(bool bDebug, const FVector &InVelocity)
{
	if (!bDebug) return;
	FVector NewPos = (InVelocity * FixedFrameTime) + FighterPawnRef->GetActorLocation();
	DrawDebugLine(GetWorld(), FighterPawnRef->GetActorLocation(), NewPos, FColor::Cyan, false, 1.f, 0, 2.f);
}

void UKnockbackState::OnExit()
{
	if (!MoveComp->IsGrounded())
	{
		MoveComp->SetVelocity(FVector(0.f, 0.f, MoveComp->GetVelocity().Z));
		MoveComp->SetMovementMode(EFighterMovementMode::Falling);
		FighterPawnRef->SetCurrentAction(bTumble ? "Tumble" : "Falling", 8);
	}
	else FighterPawnRef->SetCurrentAction("Idle", 8);
	MoveComp->bDoCollisionChecks = true;
}