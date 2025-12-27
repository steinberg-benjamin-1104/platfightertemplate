#pragma once

#include "CoreMinimal.h"
#include "FighterState.h"
#include "SkidState.generated.h"

UCLASS()
class PFTEMPLATE_API USkidState : public UFighterState
{
	GENERATED_BODY()

public:
	virtual void OnEnter() override;
	virtual bool HandlePhysics() override;
	virtual void HandleInput() override;
	virtual bool HandleTimer(int32 FramesInState) override;

	virtual FString GetStateName() override {return "Skid";}

private:
	FFixed_32 Reduction;
	int32 SkidDuration = 28;
};
