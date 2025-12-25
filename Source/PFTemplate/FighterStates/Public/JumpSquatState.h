//add short hop macro implementation (later)
#pragma once

#include "CoreMinimal.h"
#include "FighterState.h"

#include "JumpSquatState.generated.h"

UCLASS()
class PFTEMPLATE_API UJumpSquatState : public UFighterState
{
	GENERATED_BODY()

public:

	virtual void OnEnter(FFighterInput& Input) override;
	virtual bool HandlePhysics(FFighterInput& Input) override;
	virtual bool HandleTimer(FFighterInput& Input, int32 FramesInState) override;
	virtual bool HandleButtonInput(FFighterInput& Input) override;

	virtual FString GetStateName() override {return "JumpSquat";}

private:
	bool bJumpButtonHeld = false;	
	int Duration = 4;
};