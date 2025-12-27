#pragma once

#include "CoreMinimal.h"
#include "FighterState.h"
#include "BaseAttackState.generated.h"

UCLASS()
class PFTEMPLATE_API UBaseAttackState : public UFighterState
{
	GENERATED_BODY()

public:
	virtual bool HandleTimer(, int32 FramesInState) override;
	virtual bool HandlePhysics() override;
	virtual void OnExit() override;
	virtual FString GetStateName() override {return "Attack";}
};