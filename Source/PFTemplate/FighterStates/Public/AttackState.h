#pragma once

#include "CoreMinimal.h"
#include "FighterState.h"
#include "AttackState.generated.h"

UCLASS()
class PFTEMPLATE_API UAttackState : public UFighterState
{
	GENERATED_BODY()

public:
	virtual void Tick() override;
	virtual void OnExit() override;
	virtual bool Attack() override;
	virtual FString GetStateName() override {return "Attack";}
};