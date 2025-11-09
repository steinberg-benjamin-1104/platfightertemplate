#pragma once

#include "CoreMinimal.h"
#include "FighterState.h"

#include "RunState.generated.h"

class UFighterMovementComponent;
UCLASS()
class PFTEMPLATE_API URunState : public UFighterState
{
	GENERATED_BODY()

public:
	virtual void OnEnter() override;
	virtual void Tick() override;
	virtual bool JumpPressed() override;
	virtual bool ShieldPressed() override;
	virtual bool Attack() override;
	
	virtual FString GetStateName() override {return "Running";}

private:
	void ApplyRunVelocity(UFighterMovementComponent* MoveComp);
	void HandleInputTransitions(UFighterMovementComponent* MoveComp);
};