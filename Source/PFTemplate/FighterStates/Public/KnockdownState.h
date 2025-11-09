#pragma once

#include "CoreMinimal.h"
#include "FighterState.h"
#include "KnockdownState.generated.h"

UCLASS()
class PFTEMPLATE_API UKnockdownState : public UFighterState
{
	GENERATED_BODY()

public:

	virtual void OnEnter() override;
	virtual void OnExit() override;
	virtual void Tick() override;
	virtual bool Attack() override;
	virtual bool ShieldPressed() override;

	virtual FString GetStateName() override {return "Knockdown";}
};