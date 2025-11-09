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
	virtual void OnEnter() override;
	virtual void Tick() override;
	virtual bool JumpPressed() override;
	virtual bool ShieldReleased() override;
	virtual void OnExit() override;

	virtual FString GetStateName() override {return "Shield";}

private:
	UPROPERTY() UShieldComponent* ShieldComponent;
	int StartupFrames = 4;
	int EndLagFrames = 10;
	int MinHoldFrames = 2;
	int FramesToEnd;
	bool InEndLag = false;
};