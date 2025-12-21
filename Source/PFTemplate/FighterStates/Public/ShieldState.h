#pragma once

#include "CoreMinimal.h"
#include "FighterState.h"
#include "ShieldState.generated.h"

class UShieldComponent;
UCLASS()
class PFTEMPLATE_API UShieldState : public UFighterState
{
	GENERATED_BODY()

public:

	virtual void InitState(AFighterPawn* InFighterPawn, UFighterMovementComponent* InMoveComp, UFighterStateMachine* InStateMachine) override;
	virtual void OnEnter(FFighterInput& Input) override;
	virtual bool HandleTimer(FFighterInput& Input, int32 FramesInState) override;
	virtual bool HandleButtonInput(FFighterInput& Input) override;
	virtual bool HandleStickInput(FFighterInput& Input) override;
	virtual bool HandlePhysics(FFighterInput& Input) override;
	virtual void OnExit() override;

	virtual FString GetStateName() override {return "Shield";}

private:
	UPROPERTY() UShieldComponent* ShieldComponent;
	int32 StartupFrames = 4;
	int32 EndLagFrames = 10;
	int32 MinHoldFrames = 2;
	int32 FramesToEnd;
	bool InEndLag = false;
};