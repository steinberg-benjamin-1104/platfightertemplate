#pragma once

#include "CoreMinimal.h"
#include "FighterState.h"
#include "KnockdownState.generated.h"

UCLASS()
class PFTEMPLATE_API UKnockdownState : public UFighterState
{
	GENERATED_BODY()

public:
	virtual FString GetStateName() override {return "Knockdown";}
};