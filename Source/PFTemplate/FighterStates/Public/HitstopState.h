#pragma once

#include "CoreMinimal.h"
#include "FighterState.h"

#include "HitstopState.generated.h"

UCLASS()
class PFTEMPLATE_API UHitstopState : public UFighterState
{
	GENERATED_BODY()

public:
	virtual void OnEnter() override;
	virtual void Tick() override;
	virtual void OnExit() override;

	virtual FString GetStateName() override {return "Hitstop";}

private:
	int CalcHitstop();
	int Duration = 0;
};