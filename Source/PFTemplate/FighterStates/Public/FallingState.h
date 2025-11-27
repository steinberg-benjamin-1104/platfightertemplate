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
	virtual void OnEnter(FFighterInput& NewInput) override;
	virtual bool HandleButtonInput(FFighterInput& NewInput) override;
	virtual bool HandlePhysics(FFighterInput& Input) override;
	virtual bool HandleStickInput(FFighterInput& Input) override;
	virtual void OnExit() override;
	virtual FString GetStateName() override {return "Falling";}
	
private:
	ALedge* DetectNearbyLedge() const;
};