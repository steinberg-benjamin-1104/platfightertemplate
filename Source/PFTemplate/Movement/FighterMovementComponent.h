#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FighterCapsule.h"
#include "Jump.h"
#include "SafeMath.h"
#include "FixedCollision.h"
#include "FighterMovementMode.h"
#include "FighterMovementComponent.generated.h"

USTRUCT()
struct FPhysicsSnapshot
{
	GENERATED_BODY()

	FFixedVector2D Location;
	FFixedVector2D Velocity;

	EJumpType CachedJumpType;
	int32 JumpsRemaining = 0;
	UPROPERTY() EFighterMovementMode CurrentMovementMode = EFighterMovementMode::Falling;
	
	bool bOnPlatform = false;
	bool bIgnorePlatform = false;
	bool bStopMovementUpdates = false;
	bool bCanLedge = true;
	bool bDoCollisionChecks = true;
	bool bIsFastFalling = false;
};

class AFighterPawn;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PFTEMPLATE_API UFighterMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFighterMovementComponent();

	FPhysicsSnapshot PhysicsSnapshot;

	void SetVelocity(const FFixedVector2D& InVelocity) { PhysicsSnapshot.Velocity = InVelocity; }
	FFixedVector2D GetVelocity() const { return PhysicsSnapshot.Velocity; }
	
	EFighterMovementMode GetCurrentMode() const {return PhysicsSnapshot.CurrentMovementMode;}
	virtual void SetMovementMode(EFighterMovementMode NewMode);
	
	void InitFMC(AFighterPawn* InFighterPawn);
	void TickFMC();

	virtual void ApplyMovementPhysics();
	
	void DisplayDebug();

	UFUNCTION(BlueprintPure)
	bool IsGrounded() const {return PhysicsSnapshot.CurrentMovementMode == EFighterMovementMode::Grounded;}
	
	UFUNCTION(BlueprintPure)
	bool IsAirborne() { return PhysicsSnapshot.CurrentMovementMode == EFighterMovementMode::Falling ||
								PhysicsSnapshot.CurrentMovementMode == EFighterMovementMode::JumpingUp; }
	
	// Jumping
	bool StartJump(EJumpType HopType);
	bool StartGroundJump() { return StartJump(PhysicsSnapshot.CachedJumpType); }
	
	void ResetJumpCount();
	void SetMaxJumpCount(int32 NewMaxJumpCount);

	UPROPERTY(BlueprintReadWrite) int JumpRiseFramesRemaining = 0;
	
	// Air & Gravity
	void ApplyFallingGravity();
	void PerformCollisionChecks(FFixedVector2D &InVelocity);
	FFixedHitResult PerformWallCollisionCheck(FFixedVector2D &InVelocity);
	FFixedHitResult PerformGroundCollisionCheck(FFixedVector2D &InVelocity);
	FFixedHitResult PerformCeilingCollisionCheck(FFixedVector2D &InVelocity);
	
	void UpdateJumpRise();
	void ApplyAirDrift(FFixed_32 StickX);
	
	//Divide by 36000 to get Smash Values
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Gravity = 3600.f;
	//Divide by 600 to get Smash Values
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float TerminalFallVelocity = -900.f;
	//Divide by 3600 to get Smash Values
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float AirFriction = 360.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement") float FastFallMultiplier = 1.6f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) float AirAcceleration = 3000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MaxAirSpeed = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EJumpType, FJumpData> JumpDataMap;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement") float RunSpeed = 1200.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement") float DashSpeed = 1400.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement") int DashDuration = 12;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement") float WalkSpeed = 720.f;

	//Lower Friction: 0.95, Higher Friction: 0.5
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement") float GroundTraction = 36.f; //melee value * 600
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement") int32 MaxJumpCount = 2;
	
	void ApplyGroundFriction();
	
	void SnapToNearestGroundBehindStep(int32 inDirection);
	FFixedVector2D FindFurthestGroundedPosition(int32 Direction) const;
	
	void ProcessLanded();

	bool WillStayGroundedNextFrame(FFixed_32 HorizontalSpeed, int32 Direction) const;
	void HaltHorizontalVelocity();
	void HaltVerticalVelocity();
	void StopMovementCompletely(bool bStopCollision = false);
	
	void PreventLedgeFall(FFixedVector2D& InVelocity, bool bPreventFall);
	void PreventLedgeFall(bool bPreventFall) { PreventLedgeFall(PhysicsSnapshot.Velocity, bPreventFall); }

	bool IsStandingOnFacingLedge() const;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CollisionCapsule")
	FFighterCapsule CollisionCapsule;
	
	FFixedVector2D GetCenter() const {return CollisionCapsule.GetCenter();}

	void ManualDisplacement(FFixedVector2D Movement, bool bPreventLedgeFall);

	void TestIsGrounded();

protected:

	UPROPERTY() AFighterPawn* FighterPawnRef;
	
	UPROPERTY() FFighterCapsule FollowCapsule;

	FFixed_32 RisingGravity;
};