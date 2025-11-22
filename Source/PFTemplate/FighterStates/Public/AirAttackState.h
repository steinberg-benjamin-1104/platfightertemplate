#pragma once

#include "CoreMinimal.h"
#include "FighterState.h"
#include "AirAttackState.generated.h"

UCLASS()
class PFTEMPLATE_API UAirAttackState : public UFighterState
{
	GENERATED_BODY()

public:
	virtual void Tick(FFighterInput &NewInput) override;

	virtual FString GetStateName() override { return "AirAttack"; }
};