#pragma once

#include "CoreMinimal.h"
#include "FighterState.h"
#include "FighterStateMachine.h"
#include "FixedCollision.h"
#include "SafeMath.h"

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
	void CalcKBPosUpdate(FFixedVector2D &InVelocity);
	void CompleteKBPosUpdate(FFixedVector2D &InVelocity);

private:
	void InitKnockback(int32 health, int32 damage, FFixed_32 KBG, int32 BKB, FFixed_32 Angle, int32 xDir);
	FFixedVector2D CalcReflect(const FFixedVector2D& InVel, const FFixedVector2D& Normal);
	
	int32 Duration;
	bool bTumble = false;
	bool bSliding = false;
	FFixed_32 weight = 100.f;
	FFixed_32 gravity = 98.f;
	bool IsMeteor() {return (LaunchAngle >= FFixed_32(225.f) && LaunchAngle <= FFixed_32(315.f)) ||
							(LaunchAngle <= FFixed_32(-45.f) && LaunchAngle >= FFixed_32(-135.f));}
	
	FFixed_32 LaunchSpeed;
	FFixed_32 LaunchAngle;
	FFixedHitResult DoCollisionCheck(FFixedVector2D &InVelocity);

	void CheckTumble() {bTumble = (Duration - StateMachine->FramesInState) > 30;}
	void ShowDebugKB(bool bDebug, const FFixedVector2D &InVelocity);
};