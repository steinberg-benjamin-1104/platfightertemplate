#pragma once

#include "CoreMinimal.h"
#include "FighterState.h"
#include "OnLedgeState.generated.h"

UCLASS()
class PFTEMPLATE_API UOnLedgeState : public UFighterState
{
	GENERATED_BODY()

public:

	virtual void OnEnter(FFighterInput& Input) override;
	virtual void OnExit() override;
	virtual bool HandleTimer(FFighterInput& Input, int32 FramesInState) override;
	virtual bool HandleButtonInput(FFighterInput& Input) override;
	virtual FString GetStateName() override {return "On Ledge";}

private:
	int DelayAction = 30;
	bool Actionable = false;
};