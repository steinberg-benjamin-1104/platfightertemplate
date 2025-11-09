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

	virtual void OnEnter() override;
	virtual void Tick() override;
	virtual bool JumpReleased() override;
	virtual void OnExit() override;

	virtual FString GetStateName() override {return "JumpSquat";}

private:
	bool bJumpButtonHeld = false;	
	int Duration = 4;
};