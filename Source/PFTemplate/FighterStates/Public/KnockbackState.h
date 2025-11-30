#pragma once

#include "CoreMinimal.h"
#include "FighterState.h"
#include "FighterStateMachine.h"

#include "KnockbackState.generated.h"

UCLASS()
class PFTEMPLATE_API UKnockbackState : public UFighterState
{
	GENERATED_BODY()

public:
	virtual void InitState(AFighterPawn* InFighterPawn, UFighterMovementComponent* InMoveComp, UFighterStateMachine* InStateMachine) override;
	virtual void OnEnter(FFighterInput& Input) override;
	virtual void OnExit() override;
	virtual bool HandleTimer(FFighterInput& Input, int32 FramesInState);
	virtual bool HandlePhysics(FFighterInput& Input) override;

	virtual FString GetStateName() override {return "KnockbackState";}

protected:
	void CalcKBPosUpdate(FVector &InVelocity);
	void CompleteKBPosUpdate(FVector &InVelocity);

private:
	void InitKnockback(int32 health, int32 damage, FIXED_32 KBG, int32 BKB, FIXED_32 Angle, int32 xDir);
	FVector CalcReflect(const FVector& InVel, const FVector& Normal);
	
	int32 Duration;
	bool bTumble = false;
	bool bSliding = false;
	FIXED_32 weight = 100.f;
	FIXED_32 gravity = 98.f;
	bool IsMeteor() {return (LaunchAngle >= 225.f && LaunchAngle <= 315.f) || (LaunchAngle <= -45.f && LaunchAngle >= -135.f);}
	
	FIXED_32 LaunchSpeed;
	FIXED_32 LaunchAngle;
	FHitResult DoCollisionCheck(FVector &InVelocity);

	void CheckTumble() {bTumble = (Duration - StateMachine->FramesInState) > 30;}
	void ShowDebugKB(bool bDebug, const FVector &InVelocity);
};