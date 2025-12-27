#pragma once

#include "CoreMinimal.h"
#include "FighterState.h"

#include "RunState.generated.h"

class UFighterMovementComponent;
UCLASS()
class PFTEMPLATE_API URunState : public UFighterState
{
	GENERATED_BODY()

public:
	virtual void OnEnter() override;
	virtual void HandleInput();
	virtual bool HandlePhysics() override;
	
	virtual FString GetStateName() override {return "Running";}
};