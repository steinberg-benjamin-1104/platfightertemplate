#pragma once

#include "CoreMinimal.h"
#include "FighterState.h"
#include "AirAttackState.generated.h"

UCLASS()
class PFTEMPLATE_API UAirAttackState : public UFighterState
{
	GENERATED_BODY()

public:
	bool HandleTimer(FFighterInput& Input, int32 FramesInState) override;
	bool HandlePhysics(FFighterInput& Input) override;
	bool HandleStickInput(FFighterInput& Input) override;

	virtual FString GetStateName() override { return "AirAttack"; }
};