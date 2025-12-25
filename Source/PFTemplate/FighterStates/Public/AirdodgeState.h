#pragma once

#include "CoreMinimal.h"
#include "FighterState.h"
#include "AirdodgeState.generated.h"

UCLASS()
class PFTEMPLATE_API UAirDodgeState : public UFighterState
{
	GENERATED_BODY()

public:
	void OnEnter(FFighterInput& Input) override;
	bool HandleTimer(FFighterInput& Input, int32 FramesInState) override;
	bool HandlePhysics(FFighterInput& Input) override;

private:
	int32 MvmtDuration = 13;
	int32 Duration = 30;
	FFixed_32 Speed = 1200.f;
	bool bIsGrounded = false;
};