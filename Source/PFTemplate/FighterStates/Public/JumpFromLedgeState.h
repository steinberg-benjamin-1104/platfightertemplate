#pragma once

#include "CoreMinimal.h"
#include "FighterState.h"
#include "JumpFromLedgeState.generated.h"

UCLASS()
class PFTEMPLATE_API UJumpFromLedgeState : public UFighterState
{
	GENERATED_BODY()

public:
	bool HandleTimer(int32 FramesInState) override;
};