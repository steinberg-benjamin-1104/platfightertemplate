#pragma once

#include "CoreMinimal.h"
#include "FighterState.h"
#include "Input.h"

#include "DashState.generated.h"

class UFighterMovementComponent;
UCLASS()
class PFTEMPLATE_API UDashState : public UFighterState
{
	GENERATED_BODY()

public:

	virtual void InitState(AFighterPawn* InFighterPawn, UFighterMovementComponent* InMoveComp, UFighterStateMachine* InStateMachine) override;
	virtual void OnEnter() override;
	virtual void Tick() override;
	virtual void OnExit() override;
	virtual bool JumpPressed() override;
	virtual bool ShieldPressed() override;
	virtual bool Attack() override;

	virtual FString GetStateName() override {return "Dash";}

private:
	float DashDirection;
	bool bHasSnappedToLedge = false;
	int DashDuration;

	void StartDash();
	void ApplyDashVelocity();
	void HandleDashEnd(const FStickInt8& StickInput);
};