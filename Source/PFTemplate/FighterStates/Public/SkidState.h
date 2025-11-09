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
	virtual void Tick() override;
	virtual bool JumpPressed() override;

	virtual FString GetStateName() override {return "Skid";}

private:
	int Duration = 25;
	float SkidFriction = 0.f;
	float InitialSkidDirection = 0.f;
};
