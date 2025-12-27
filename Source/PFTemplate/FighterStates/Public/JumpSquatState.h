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
	virtual bool HandlePhysics() override;
	virtual bool HandleTimer(int32 FramesInState) override;
	virtual void HandleInput() override;

	virtual FString GetStateName() override {return "JumpSquat";}

private:
	bool bJumpButtonHeld = false;	
	int Duration = 4;
};