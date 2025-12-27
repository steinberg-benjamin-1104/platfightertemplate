// FallingState.h
#pragma once

#include "CoreMinimal.h"
#include "FighterState.h"
#include "FallingState.generated.h"

class ALedge;

UCLASS()
class PFTEMPLATE_API UFallingState : public UFighterState
{
	GENERATED_BODY()

public:
	virtual void OnEnter() override;
	virtual void HandleInput() override;
	virtual bool HandlePhysics() override;
	virtual void Tick(int32 FramesInState) override;
	virtual FString GetStateName() override {return "Falling";}
	
private:
	ALedge* DetectNearbyLedge() const;
};