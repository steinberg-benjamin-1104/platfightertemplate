#pragma once

#include "CoreMinimal.h"
#include "FallingState.h"
#include "PlatformDrop.generated.h"

UCLASS()
class PFTEMPLATE_API UPlatformDropState : public UFallingState
{
	GENERATED_BODY()

public:
	virtual void OnEnter(FFighterInput& Input) override;
	virtual void OnExit() override;
	virtual bool HandleTimer(FFighterInput& Input, int32 FramesInState) override;
	
	virtual FString GetStateName() override {return "JumpUp";}

private:
	int32 Duration = 5;
};