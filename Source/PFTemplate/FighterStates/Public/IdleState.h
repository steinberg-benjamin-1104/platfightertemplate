#pragma once

#include "CoreMinimal.h"
#include "FighterState.h"
#include "IdleState.generated.h"

UCLASS()
class PFTEMPLATE_API UIdleState : public UFighterState
{
	GENERATED_BODY()

public:
	
	virtual void OnExit() override;
	virtual bool HandleButtonInput() override;
	virtual bool HandleStickInput() override;
	virtual bool HandlePhysics() override;

	virtual FString GetStateName() override {return "Idle";}

private:
	bool bOnLedge = false;
};