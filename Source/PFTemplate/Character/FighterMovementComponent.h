#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FighterCapsule.h"
#include "Jump.h"
#include "SafeMath.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.generated.h"

UENUM(BlueprintType)
enum class EFighterMovementMode : uint8
{
	Grounded UMETA(DisplayName = "Grounded"),
	JumpingUp UMETA(DisplayName = "Jumping Up"),
	Falling UMETA(DisplayName = "Falling"),
	None UMETA(DisplayName = "None")
};

class AFighterPawn;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PFTEMPLATE_API UFighterMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFighterMovementComponent();

	FFixedVector2D Velocity;

	UFUNCTION(BlueprintCallable)
	EFighterMovementMode GetCurrentMode() {return CurrentMovementMode;}

	UFUNCTION(BlueprintCallable)
	virtual void SetMovementMode(EFighterMovementMode NewMode);
	
	void InitFMC(AFighterPawn* InFighterPawn);
	void TickFMC();

	virtual void ApplyMovementPhysics();
	
	void DisplayDebug();

	UFUNCTION(BlueprintPure)
	bool IsGrounded() const {return CurrentMovementMode == EFighterMovementMode::Grounded;}
	UFUNCTION(BlueprintPure)
	bool IsAirborne() {return CurrentMovementMode == EFighterMovementMode::Falling || CurrentMovementMode == EFighterMovementMode::JumpingUp;}
	
	// Jumping
	bool DoHop(EHopType HopType);
	void ResetJumpCount();
	void SetMaxJumpCount(int32 NewMaxJumpCount);

	UPROPERTY(BlueprintReadWrite) int JumpRiseFramesRemaining = 0;
	
	// Air & Gravity
	void ApplyFallingGravity();
	void PerformCollisionChecks();
	bool PerformWallCollisionCheck(FFixedVector2D &InVelocity, FFixedHitResult& OutHit);
	bool PerformGroundCollisionCheck(FFixedVector2D &InVelocity, FFixedHitResult& OutHit, bool bHitWall);
	bool PerformCeilingCollisionCheck(FFixedVector2D &InVelocity, FFixedHitResult& OutHit, bool bHitWall);
	
	void UpdateJumpRise();
	void ApplyAirDrift(FIXED_32 StickX);
	
	//Divide by 3600 to get Smash Values
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FIXED_32 Gravity = 3600.f;
	//Divide by 600 to get Smash Values
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FIXED_32 TerminalFallVelocity = -900.f;
	//Divide by 3600 to get Smash Values
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FIXED_32 AirFriction = 360.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement") FIXED_32 FastFallMultiplier = 1.6f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FIXED_32 AirAcceleration = 3000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FIXED_32 MaxAirSpeed = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EHopType, FHopData> HopDataMap;

	UFUNCTION(BlueprintCallable)
	void SetVelocity(const FFixedVector2D& InVelocity) { Velocity = InVelocity; }

	UFUNCTION(BlueprintCallable)
	FFixedVector2D GetVelocity() const { return Velocity; }
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement") FIXED_32 RunSpeed = 1200.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement") FIXED_32 DashSpeed = 1400.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement") int DashDuration = 12;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement") FIXED_32 WalkSpeed = 500.f;

	//Lower Friction: 0.95, Higher Friction: 0.5
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement") FIXED_32 GroundFriction = 0.8f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement") int MaxJumpCount = 2;
	UPROPERTY() int JumpsRemaining = 0;
	UPROPERTY() bool bIsFastFalling = false;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void ApplyGroundFriction();

	bool bCanApplyGroundFriction = true;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void ApplyCustomFriction(FIXED_32 Friction);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void SnapToNearestGroundBehindStep(int32 inDirection);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	FFixedVector2D FindFurthestGroundedPosition(int32 Direction) const;
	
	void ProcessLanded();

	bool WillStayGroundedNextFrame(FIXED_32 HorizontalSpeed, int32 Direction) const;
	void HaltHorizontalVelocity();
	void HaltVerticalVelocity();
	void StopMovementCompletely(bool bStopCollision = false);
	
	void HandleLedgeOrFall(bool bPreventFall);

	bool IsStandingOnFacingLedge() const;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CollisionCapsule")
	FFighterCapsule CollisionCapsule;
	
	FFixedVector2D GetCenter() const {return CollisionCapsule.GetCenter();}
	
	void UpdateCapsule();

	bool bDoCollisionChecks = true;

	EFighterMovementMode CurrentMovementMode = EFighterMovementMode::Falling;

protected:
	
	UPROPERTY() AFighterPawn* FighterPawnRef = nullptr;

	UPROPERTY() FHopData CurrentHopData;
	int HopCurrentFrame = 0;

	FFighterCapsule FollowCapsule;

private:
	void DrawDebugFighterCapsule(const FFighterCapsule& Capsule, const FColor& Color);
};