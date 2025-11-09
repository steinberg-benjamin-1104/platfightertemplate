#pragma once

#include "CoreMinimal.h"
#include "FighterState.h"

#include "HeavyAttackState.generated.h"

UCLASS()
class PFTEMPLATE_API UHeavyAttackState : public UFighterState
{
	GENERATED_BODY()

public:
	
	virtual void OnEnter() override;
	virtual void Tick() override;
	virtual void OnExit() override;
	virtual bool AttackRelease() override;

	virtual FString GetStateName() override {return "Charging Heavy";}
	
private:
	int MaxHoldDuration = 59;
	bool bEarlyRelease = false;
};
