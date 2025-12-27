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

	virtual void InitState(AFighterPawn* InFighterPawn, UFighterMovementComponent* InMoveComp, UFighterStateMachine* InStateMachine, FInputBuffer* InBuffer) override;
	virtual void OnEnter() override;
	virtual bool HandlePhysics() override;
	virtual void HandleInput() override;
	virtual bool HandleTimer(int32 FramesInState) override;

	virtual FString GetStateName() override {return "Dash";}

private:
	float DashDirection;
	bool bHasSnappedToLedge = false;
	int DashDuration;
};
