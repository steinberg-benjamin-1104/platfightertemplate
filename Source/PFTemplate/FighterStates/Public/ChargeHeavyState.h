#pragma once

#include "CoreMinimal.h"
#include "FighterState.h"

#include "ChargeHeavyState.generated.h"

UCLASS()
class PFTEMPLATE_API UChargeHeavyState : public UFighterState
{
	GENERATED_BODY()

public:
	
	virtual void OnEnter() override;
	virtual bool HandleTimer(int32 FramesInState) override;
	virtual void HandleInput() override;
	virtual bool HandlePhysics() override;
	virtual void OnExit() override;

	virtual FString GetStateName() override {return "Charging Heavy";}
	
private:
	int MaxHoldDuration = 59;
};
