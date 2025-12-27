#pragma once

#include "CoreMinimal.h"
#include "FallingState.h"
#include "PlatformDrop.generated.h"

UCLASS()
class PFTEMPLATE_API UPlatformDropState : public UFallingState
{
	GENERATED_BODY()

public:
	virtual void OnEnter() override;
	virtual void OnExit() override;
	virtual bool HandleTimer(int32 FramesInState) override;
	
	virtual FString GetStateName() override {return "PlatformDrop";}

private:
	int32 Duration = 5;
};