#include "KnockbackState.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.h"
#include "FixedCollision.h"
#include "SafeMath.h"

void UKnockbackState::InitState(AFighterPawn* InFighterPawn, UFighterMovementComponent* InMoveComp, UFighterStateMachine* InStateMachine)
{
	Super::InitState(InFighterPawn, InMoveComp, InStateMachine);
	weight = InFighterPawn->Weight;
	gravity = InMoveComp->Gravity;
}

void UKnockbackState::InitKnockback(int32 h, int32 d, FFixed_32 KBG, int32 BKB, FFixed_32 a, int32 x)
{
	const FFixed_32 KnockbackValue = (((h/10 + (h * d/20)) * (FFixed_32(200.f)
									/ (weight + FFixed_32(100.f)) * FFixed_32(1.4f))
									+ FFixed_32(18.f)) * KBG) + BKB;

	LaunchSpeed = KnockbackValue * FFixed_32(0.3f);
	if (x < 0) LaunchAngle = FFixed_32(180.f) - a;
	else LaunchAngle = a;
	Duration = FixedFloor(KnockbackValue * FFixed_32(0.4f));
	CheckTumble();
}

void UKnockbackState::OnEnter(FFighterInput& Input)
{
	MoveComp->bDoCollisionChecks = false;
	bSliding = false;
	
	FDamageInfo Dam = FighterPawnRef->StoredDamageInfo;
	InitKnockback(
		FighterPawnRef->Health/10,
		Dam.Damage/10,
		Dam.KnockbackGrowth.ToFixed(),
		Dam.BaseKnockback,
		Dam.Angle.ToFixed(),
		FighterPawnRef->GetFacingDirection() * -1
		);

	FighterPawnRef->SetCurrentAnimation("Knockback", Duration);
}

bool UKnockbackState::HandleTimer(FFighterInput& Input, int32 FramesInState)
{
	if (StateMachine->FramesInState == Duration)
	{
		StateMachine->ChangeFighterState(MoveComp->IsGrounded() ? "Idle" : bTumble ? "Tumble" : "Falling", Input);
		return true;
	}
	return false;
}

bool UKnockbackState::HandlePhysics(FFighterInput& Input)
{
	FFixedVector2D Velocity;
	CalcKBPosUpdate(Velocity);
	const FFixedVector2D SavedVelocity = Velocity;

	FFixedHitResult HitResult = DoCollisionCheck(Velocity);
	const bool bHit       = HitResult.bBlockingHit;
	const bool bGroundHit = HitResult.Normal.Z >= FFixed_32(0.7f);
	const bool bIsMeteor  = IsMeteor();
	
	// Wall Bounce
	if (bTumble && bHit && !bGroundHit)
	{
		LaunchSpeed *= FFixed_32(0.9f);
		Velocity *= FFixed_32(0.9f);
		Velocity = CalcReflect(SavedVelocity, HitResult.Normal);
	}

	// Ground Bounce
	else if (bTumble && bHit && bGroundHit && bIsMeteor)
	{
		Velocity = CalcReflect(SavedVelocity, HitResult.Normal);
		LaunchSpeed *= FFixed_32(0.4f);
		Velocity *= FFixed_32(0.4f);
	}
	
	// Begin Slide
	else if (bHit && bGroundHit && !bSliding && !bTumble)
	{
		UE_LOG(LogTemp, Warning, TEXT("EnterSlide"));
		MoveComp->SetMovementMode(EFighterMovementMode::Grounded);
		LaunchAngle = FixedRadiansToDegrees(FixedAtan2(Velocity.Z, Velocity.X)
);

		bSliding = true;
		FighterPawnRef->SetCurrentAnimation("Sliding");

		Velocity *= FFixed_32(0.95f);
		LaunchSpeed *= FFixed_32(0.95f);
	}

	// Slide Physics
	if (bSliding)
	{
		Velocity.X *= MoveComp->GroundFriction;
		Velocity.X = FixedClamp(Velocity.X, FFixed_32(-498.f), FFixed_32(498.f)); // 8.3 * 60
	}

	CompleteKBPosUpdate(Velocity);
	return false;
}

void UKnockbackState::CalcKBPosUpdate(FFixedVector2D& InVelocity)
{
	if (LaunchSpeed <= FFixed_32(0)) 
		LaunchSpeed = FFixed_32(0);
	
	FFixed_32 Radians = FixedDegreesToRadians(LaunchAngle);
	
	FFixedVector2D PosUpdate(
		Radians.Cos() * LaunchSpeed,
		Radians.Sin() * LaunchSpeed
	);
	
	InVelocity = PosUpdate * FFixed_32(60.f);
	
	if (InVelocity.Z > MoveComp->TerminalFallVelocity) InVelocity.Z -= gravity * FixedDt;
	LaunchAngle = FixedRadiansToDegrees(FixedAtan2(InVelocity.Z, InVelocity.X));
}

FFixedVector2D UKnockbackState::CalcReflect(const FFixedVector2D& InVel, const FFixedVector2D& Normal)
{
	FFixedVector2D NewAngle = InVel - FFixed_32(2.f) * InVel.Dot(Normal) * Normal;
	LaunchAngle = FixedRadiansToDegrees(FixedAtan2(NewAngle.Z, NewAngle.X));
	CheckTumble();
	return NewAngle;
}

FFixedHitResult UKnockbackState::DoCollisionCheck(FFixedVector2D &InVelocity)
{
	FFixedHitResult CeilingHit, GroundHit, WallHit;
	
	bool bGround = MoveComp->PerformGroundCollisionCheck(InVelocity, GroundHit);
	bool bCeiling = MoveComp->PerformCeilingCollisionCheck(InVelocity, CeilingHit);
	bool bWall = MoveComp->PerformWallCollisionCheck(InVelocity, WallHit);

	if (bGround) return GroundHit;
	if (bWall) return WallHit;
	if (bCeiling) return CeilingHit;
	return FFixedHitResult();
}

void UKnockbackState::CompleteKBPosUpdate(FFixedVector2D& InVelocity)
{
	MoveComp->SetVelocity(InVelocity);
	LaunchSpeed -= 0.51f;
	ShowDebugKB(true, InVelocity);
}


void UKnockbackState::ShowDebugKB(bool bDebug, const FFixedVector2D &InVelocity)
{
	if (!bDebug) return;
	FVector NewPos = Fixed2DToVector((InVelocity * FixedDt) + FighterPawnRef->GetFixedLoc());
	DrawDebugLine(GetWorld(), FighterPawnRef->GetActorLocation(), NewPos, FColor::Cyan, false, 1.f, 0, 2.f);
}

void UKnockbackState::OnExit()
{
	if (!MoveComp->IsGrounded())
	{
		MoveComp->HaltHorizontalVelocity();
		MoveComp->SetMovementMode(EFighterMovementMode::Falling);
		FighterPawnRef->SetCurrentAnimation(bTumble ? "Tumble" : "Falling", 8);
	}
	else FighterPawnRef->SetCurrentAnimation("Idle", 8);
	MoveComp->bDoCollisionChecks = true;
}