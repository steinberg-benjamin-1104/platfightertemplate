#pragma once

#include "CoreMinimal.h"
#include "FallingState.h"
#include "RisingState.generated.h"

UCLASS()
class PFTEMPLATE_API URisingState : public UFallingState
{
	GENERATED_BODY()

public:
	virtual void OnEnter() override;
	virtual void Tick() override;
	
	virtual FString GetStateName() override {return "JumpRise";}
};