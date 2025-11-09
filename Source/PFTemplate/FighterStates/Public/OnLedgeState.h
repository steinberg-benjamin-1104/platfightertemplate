#pragma once

#include "CoreMinimal.h"
#include "FighterState.h"
#include "OnLedgeState.generated.h"

UCLASS()
class PFTEMPLATE_API UOnLedgeState : public UFighterState
{
	GENERATED_BODY()

public:

	virtual void OnEnter() override;
	virtual void OnExit() override;
	virtual void Tick() override;
	virtual bool Attack() override;
	virtual bool ShieldPressed() override;
	virtual bool JumpPressed() override;

	virtual FString GetStateName() override {return "On Ledge";}

private:
	int DelayAction = 60;
	bool Actionable = false;
};