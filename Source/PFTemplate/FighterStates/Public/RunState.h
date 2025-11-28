#pragma once

#include "CoreMinimal.h"
#include "FighterState.h"

#include "RunState.generated.h"

class UFighterMovementComponent;
UCLASS()
class PFTEMPLATE_API URunState : public UFighterState
{
	GENERATED_BODY()

public:
	virtual void OnEnter(FFighterInput& Input) override;
	virtual bool HandleButtonInput(FFighterInput& Input) override;
	virtual bool HandleStickInput(FFighterInput& Input) override;
	virtual bool HandlePhysics(FFighterInput& Input) override;
	
	virtual FString GetStateName() override {return "Running";}
};