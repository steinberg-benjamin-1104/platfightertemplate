#pragma once

#include "CoreMinimal.h"
#include "FighterState.h"
#include "ShieldStartupState.generated.h"

UCLASS()
class PFTEMPLATE_API UShieldStartupState : public UFighterState 
{
	GENERATED_BODY()
public:

	virtual void OnEnter() override;
	virtual bool HandlePhysics() override;
	virtual bool HandleTimer(int32 FramesInState) override;
	virtual void HandleInput() override;

	virtual FString GetStateName() override {return "ShieldStartup";}

private:
	int32 Duration = 4;
};