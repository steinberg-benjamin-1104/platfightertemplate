#pragma once

#include "CoreMinimal.h"
#include "FighterState.h"
#include "AirAttackState.generated.h"

UCLASS()
class PFTEMPLATE_API UAirAttackState : public UFighterState
{
	GENERATED_BODY()

public:
	bool HandleTimer(int32 FramesInState) override;
	bool HandlePhysics() override;
	void HandleInput() override;

	virtual FString GetStateName() override { return "AirAttack"; }
};