#pragma once

#include "CoreMinimal.h"
#include "FighterState.h"
#include "SpecialState.generated.h"

UCLASS()
class PFTEMPLATE_API USpecialState : public UFighterState
{
	GENERATED_BODY()

public:
	virtual void OnEnter(FFighterInput& Input) override;
	virtual bool HandleTimer(FFighterInput &NewInput, int32 FramesInState) override;
	virtual void OnExit() override;
	virtual FString GetStateName() override {return "Special";}
};