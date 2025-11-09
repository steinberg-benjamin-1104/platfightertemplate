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
	virtual void Tick() override;
	virtual bool Attack() override;
	virtual bool JumpPressed() override;
	virtual bool ShieldPressed() override;

	virtual FString GetStateName() override {return "Idle";}

private:
	bool bOnLedge = false;
};