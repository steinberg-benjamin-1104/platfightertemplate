#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FighterCapsule.h"
#include "Jump.h"
#include "SafeMath.h"
#include "SafeMathBP.h"
#include "FixedCollision.h"
#include "AnimMvmt.h"
#include "FighterMovementMode.h"
#include "FighterMovementComponent.generated.h"

class AFighterPawn;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PFTEMPLATE_API UFighterMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFighterMovementComponent();

	FFixedVector2D Velocity = FFixedVector2D(FFixed_32(0.f), FFixed_32(0.f));

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
	void PerformCollisionChecks(FFixedVector2D &InVelocity);
	FFixedHitResult PerformWallCollisionCheck(FFixedVector2D &InVelocity);
	FFixedHitResult PerformGroundCollisionCheck(FFixedVector2D &InVelocity);
	FFixedHitResult PerformCeilingCollisionCheck(FFixedVector2D &InVelocity);
	
	void UpdateJumpRise();
	void ApplyAirDrift(FFixed_32 StickX);
	
	//Divide by 36000 to get Smash Values
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FFixed_32BP Gravity = 3600.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FFixed_32BP RisingGravity = 3600.f;
	//Divide by 600 to get Smash Values
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FFixed_32BP TerminalFallVelocity = -900.f;
	//Divide by 3600 to get Smash Values
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FFixed_32BP AirFriction = 360.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement") FFixed_32BP FastFallMultiplier = 1.6f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FFixed_32BP AirAcceleration = 3000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FFixed_32BP MaxAirSpeed = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EHopType, FJumpData> JumpDataMap;
	
	void SetVelocity(const FFixedVector2D& InVelocity) { Velocity = InVelocity; }
	FFixedVector2D GetVelocity() const { return Velocity; }
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement") FFixed_32BP RunSpeed = 1200.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement") FFixed_32BP DashSpeed = 1400.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement") int DashDuration = 12;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement") FFixed_32BP WalkSpeed = 500.f;

	//Lower Friction: 0.95, Higher Friction: 0.5
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement") FFixed_32BP GroundFriction = 0.8f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement") int MaxJumpCount = 2;
	int JumpsRemaining = 0;
	bool bIsFastFalling = false;
	void ApplyGroundFriction();
	bool bCanApplyGroundFriction = true;
	void ApplyCustomFriction(FFixed_32 Friction);
	void SnapToNearestGroundBehindStep(int32 inDirection);
	FFixedVector2D FindFurthestGroundedPosition(int32 Direction) const;
	
	void ProcessLanded();

	bool WillStayGroundedNextFrame(FFixed_32 HorizontalSpeed, int32 Direction) const;
	void HaltHorizontalVelocity();
	void HaltVerticalVelocity();
	void StopMovementCompletely(bool bStopCollision = false);
	
	void PreventLedgeFall(FFixedVector2D& InVelocity, bool bPreventFall);
	void PreventLedgeFall(bool bPreventFall) { PreventLedgeFall(Velocity, bPreventFall); }

	bool IsStandingOnFacingLedge() const;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CollisionCapsule")
	FFighterCapsule CollisionCapsule;
	
	FFixedVector2D GetCenter() const {return CollisionCapsule.GetCenter();}

	bool bDoCollisionChecks = true;

	UPROPERTY() EFighterMovementMode CurrentMovementMode = EFighterMovementMode::Falling;

	void ManualDisplacement(FFixedVector2D Movement, bool bPreventLedgeFall);

	void ApplyAnimMovement(int32 CurrentFrame);
	void SetCurrAnimMvmt(const FAnimMvmt& InAnimMvmt) { CurrAnimMvmt = InAnimMvmt; }

protected:
	
	UPROPERTY() AFighterPawn* FighterPawnRef = nullptr;

	UPROPERTY() FJumpData CurrentHopData;
	int HopCurrentFrame = 0;

	UPROPERTY() FFighterCapsule FollowCapsule;

	UPROPERTY() FAnimMvmt CurrAnimMvmt;

private:
	void DrawDebugFighterCapsule(const FFighterCapsule& Capsule, const FColor& Color);
};