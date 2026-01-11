#pragma once
#include "CoreMinimal.h"
#include "FighterState.h"
#include "GrabHeldState.generated.h"

UCLASS()
class PFTEMPLATE_API UGrabHeldState : public UFighterState
{
	GENERATED_BODY()

public:

	virtual void OnEnter() override;

	virtual FString GetStateName() override { return "Grab Held"; }
};