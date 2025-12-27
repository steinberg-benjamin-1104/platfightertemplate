#pragma once

#include "CoreMinimal.h"
#include "FighterState.h"
#include "AirdodgeState.generated.h"

UCLASS()
class PFTEMPLATE_API UAirDodgeState : public UFighterState
{
	GENERATED_BODY()

public:
	void OnEnter() override;
	bool HandleTimer(int32 FramesInState) override;
	bool HandlePhysics() override;

private:
	int32 MvmtDuration = 10;
	int32 Duration = 28;
	FFixed_32 Speed = 1600.f;
	bool bIsGrounded = false;
};