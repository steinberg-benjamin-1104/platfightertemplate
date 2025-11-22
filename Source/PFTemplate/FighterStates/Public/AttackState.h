#pragma once

#include "CoreMinimal.h"
#include "FighterState.h"
#include "AttackState.generated.h"

UCLASS()
class PFTEMPLATE_API UAttackState : public UFighterState
{
	GENERATED_BODY()

public:
	virtual void Tick(FFighterInput &NewInput) override;
	virtual void OnExit() override;
	virtual FString GetStateName() override {return "Attack";}
};