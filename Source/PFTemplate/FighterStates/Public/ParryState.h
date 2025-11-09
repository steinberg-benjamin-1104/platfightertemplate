#pragma once

#include "CoreMinimal.h"
#include "FighterState.h"
#include "ParryState.generated.h"

UCLASS()
class PFTEMPLATE_API UParryState : public UFighterState
{
	GENERATED_BODY()

public:
	void OnEnter() override;
	void Tick() override;
	void OnExit() override;

private:
	int duration = 5;
	int recovery = 15;
	bool InEndLag = false;
};