#pragma once

#include "CoreMinimal.h"
#include "FallingState.h"
#include "JumpUpState.generated.h"

UCLASS()
class PFTEMPLATE_API UJumpUpState : public UFallingState
{
	GENERATED_BODY()

public:
	virtual bool HandleStickInput(FFighterInput& Input) override;
	virtual bool HandlePhysics(FFighterInput& Input) override;
	
	virtual FString GetStateName() override {return "JumpUp";}
};