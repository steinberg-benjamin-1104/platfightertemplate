#pragma once

#include "CoreMinimal.h"
#include "FighterState.h"
#include "BaseAttackState.generated.h"

UCLASS()
class PFTEMPLATE_API UBaseAttackState : public UFighterState
{
	GENERATED_BODY()

public:
	virtual void Tick(FFighterInput &NewInput) override;
	virtual bool HandlePhysics(FFighterInput &NewInput) override;
	virtual void OnExit() override;
	virtual FString GetStateName() override {return "Attack";}
};