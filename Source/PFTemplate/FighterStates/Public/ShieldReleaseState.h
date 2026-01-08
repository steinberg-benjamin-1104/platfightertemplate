#pragma once
#include "CoreMinimal.h"
#include "FighterState.h"
#include "ShieldReleaseState.generated.h"

UCLASS()
class PFTEMPLATE_API UShieldReleaseState : public UFighterState
{
	GENERATED_BODY()

public:

	void OnEnter() override;
	bool HandleTimer(int32 FramesInState) override;

	FString GetStateName() override { return "Shield Release"; }

private:
	int32 duration = 10;
};