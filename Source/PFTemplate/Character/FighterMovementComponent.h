#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "FighterCapsule.h"
#include "Jump.h"
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
class PFTEMPLATE_API UFighterMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

public:
	UFighterMovementComponent();

	static constexpr float FixedFrameTime = 1.0f / 60.0f;

	UFUNCTION(BlueprintCallable)
	EFighterMovementMode GetCurrentMode() {return CurrentMovementMode;}

	UFUNCTION(BlueprintCallable)
	virtual void SetMovementMode(EFighterMovementMode NewMode);

	// Core Overrides
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

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
	bool PerformWallCollisionCheck(FVector &InVelocity, FHitResult& OutHit);
	bool PerformGroundCollisionCheck(FVector &InVelocity, FHitResult& OutHit, bool bHitWall);
	bool PerformCeilingCollisionCheck(FVector &InVelocity, FHitResult& OutHit, bool bHitWall);
	
	void UpdateJumpRise();
	void ApplyAirDrift(float StickX);
	
	//Divide by 3600 to get Smash Values
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Gravity = 3600.f;
	//Divide by 600 to get Smash Values
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float TerminalFallVelocity = -900.f;
	//Divide by 3600 to get Smash Values
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float AirFriction = 360.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement") float FastFallMultiplier = 1.6f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) float AirAcceleration = 3000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MaxAirSpeed = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EHopType, FHopData> HopDataMap;
	
	UFUNCTION(BlueprintCallable)
	void SetVelocity(const FVector& InVelocity);
	FVector GetVelocity() const { return Velocity; }
	FVector PreviousVelocity;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement") float RunSpeed = 1200.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement") float DashSpeed = 1400.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement") int DashDuration = 12;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement") float WalkSpeed = 500.f;

	//Lower Friction: 0.95, Higher Friction: 0.5
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement") float GroundFriction = 0.8f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement") int MaxJumpCount = 2;
	UPROPERTY() int JumpsRemaining = 0;
	UPROPERTY() bool bIsFastFalling = false;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void ApplyGroundFriction();

	bool bCanApplyGroundFriction = true;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void ApplyCustomFriction(float Friction);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void SnapToNearestGroundBehindStep(float Direction);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	FVector FindFurthestGroundedPosition() const;
	
	void ProcessLanded();

	bool WillStayGroundedNextFrame(float HorizontalSpeed, float Direction) const; //this update
	void HaltHorizontalVelocity();
	void HaltVerticalVelocity();
	void StopMovementCompletely(bool bStopCollision = false);
	
	void HandleLedgeOrFall(bool bPreventFall);

	bool IsStandingOnFacingLedge() const;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CollisionCapsule")
	FFighterCapsule CollisionCapsule;
	
	FVector GetCenter() const {return CollisionCapsule.GetCenter();}
	
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

	FCollisionShape GetCapsule() const;
};