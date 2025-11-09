// WalkingState.h
#pragma once

#include "CoreMinimal.h"
#include "IdleState.h"
#include "WalkingState.generated.h"

UCLASS()
class PFTEMPLATE_API UWalkingState : public UIdleState
{
	GENERATED_BODY()

public:
	virtual void OnEnter() override;
	virtual void Tick() override;

	virtual FString GetStateName() override {return "Walk";}
};