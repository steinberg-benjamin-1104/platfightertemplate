#pragma once

#include "CoreMinimal.h"
#include "FighterState.h"
#include "JumpUpState.generated.h"

UCLASS()
class PFTEMPLATE_API UJumpUpState : public UFighterState
{
	GENERATED_BODY()

public:
	virtual void OnEnter() override;
	virtual bool HandleTimer(int32 FramesInState) override;
	virtual bool HandlePhysics() override;
	virtual void HandleInput() override;
	virtual void Tick(int32 FramesInState) override {UFighterState::Tick(FramesInState);}
	
	virtual FString GetStateName() override {return "JumpUp";}

private:
	int32 Duration = 1;
};