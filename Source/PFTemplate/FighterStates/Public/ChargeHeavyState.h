#pragma once

#include "CoreMinimal.h"
#include "FighterState.h"

#include "ChargeHeavyState.generated.h"

UCLASS()
class PFTEMPLATE_API UChargeHeavyState : public UFighterState
{
	GENERATED_BODY()

public:
	
	virtual void OnEnter(FFighterInput& Input) override;
	virtual bool HandleTimer(FFighterInput& Input, int32 FramesInState) override;
	virtual bool HandleButtonInput(FFighterInput& Input) override;
	virtual bool HandlePhysics(FFighterInput& Input) override;
	virtual void OnExit() override;

	virtual FString GetStateName() override {return "Charging Heavy";}
	
private:
	int MaxHoldDuration = 59;
};
