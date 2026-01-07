#pragma once

#include "CoreMinimal.h"
#include "FighterState.h"
#include "MiscAnimState.generated.h"

UCLASS()
class PFTEMPLATE_API UMiscAnimState : public UFighterState
{
	GENERATED_BODY()

public:
	virtual bool HandleTimer(int32 FramesInState) override;

	virtual FString GetStateName() override { return "MiscAnim"; }
};