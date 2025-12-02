#pragma once

#include "CoreMinimal.h"
#include "FighterState.h"
#include "SkidState.generated.h"

UCLASS()
class PFTEMPLATE_API USkidState : public UFighterState
{
	GENERATED_BODY()

public:
	virtual void OnEnter(FFighterInput& Input) override;
	virtual bool HandlePhysics(FFighterInput& Input) override;
	virtual bool HandleButtonInput(FFighterInput& Input) override;
	virtual bool HandleTimer(FFighterInput& Input, int32 FramesInState) override;

	virtual FString GetStateName() override {return "Skid";}

private:
	FFixed_32 Reduction;
	int32 SkidDuration;
};
