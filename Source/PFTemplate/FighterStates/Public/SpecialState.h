#pragma once

#include "CoreMinimal.h"
#include "FighterState.h"
#include "SpecialState.generated.h"

UCLASS()
class PFTEMPLATE_API USpecialState : public UFighterState
{
	GENERATED_BODY()

public:
	virtual void OnEnter() override;
	virtual bool HandleTimer(int32 FramesInState) override;
	virtual void OnExit() override;
	virtual FString GetStateName() override {return "Special";}
};