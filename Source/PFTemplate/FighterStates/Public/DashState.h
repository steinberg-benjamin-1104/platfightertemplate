#pragma once

#include "CoreMinimal.h"
#include "FighterState.h"

#include "DashState.generated.h"

class UFighterMovementComponent;
UCLASS()
class PFTEMPLATE_API UDashState : public UFighterState
{
	GENERATED_BODY()

public:

	virtual void InitState(AFighterPawn* InFighterPawn, UFighterMovementComponent* InMoveComp, UFighterStateMachine* InStateMachine) override;
	virtual void OnEnter(FFighterInput& NewInput) override;
	virtual bool HandleStickInput(FFighterInput& NewInput) override;
	virtual bool HandlePhysics(FFighterInput& NewInput) override;
	virtual bool HandleButtonInput(FFighterInput& NewInput) override;
	virtual bool HandleTimer(FFighterInput& NewInput, int32 FramesInState) override;

	virtual FString GetStateName() override {return "Dash";}

private:
	float DashDirection;
	bool bHasSnappedToLedge = false;
	int DashDuration;
};
