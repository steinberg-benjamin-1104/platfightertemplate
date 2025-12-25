#pragma once

#include "CoreMinimal.h"
#include "FallingState.h"
#include "JumpUpState.generated.h"

UCLASS()
class PFTEMPLATE_API UJumpUpState : public UFallingState
{
	GENERATED_BODY()

public:
	virtual void OnEnter(FFighterInput& Input) override;
	virtual bool HandleTimer(FFighterInput& Input, int32 FramesInState) override;
	virtual bool HandleStickInput(FFighterInput& Input) override;
	virtual bool HandlePhysics(FFighterInput& Input) override;
	virtual void Tick(FFighterInput& Input, int32 FramesInState) override {UFighterState::Tick(Input, FramesInState);}
	
	virtual FString GetStateName() override {return "JumpUp";}

private:
	int32 Duration = 1;
};