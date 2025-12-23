#pragma once

#include "CoreMinimal.h"
#include "FighterState.h"
#include "BaseAttackState.generated.h"

UCLASS()
class PFTEMPLATE_API UBaseAttackState : public UFighterState
{
	GENERATED_BODY()

public:
	virtual bool HandleTimer(FFighterInput &NewInput, int32 FramesInState) override;
	virtual bool HandlePhysics(FFighterInput &NewInput) override;
	virtual void OnExit() override;
	virtual FString GetStateName() override {return "Attack";}
};