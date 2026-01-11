#pragma once
#include "CoreMinimal.h"
#include "FighterState.h"
#include "GrabHoldState.generated.h"

UCLASS()
class PFTEMPLATE_API UGrabHoldState : public UFighterState
{
	GENERATED_BODY()

public:

	virtual void OnEnter() override;
	virtual bool HandlePhysics() override;

	FString GetStateName() override { return "GrabHold"; }
	
};