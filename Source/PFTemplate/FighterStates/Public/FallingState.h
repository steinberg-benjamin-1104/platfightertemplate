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
	virtual void Tick() override;
	virtual bool Attack() override;
	virtual bool JumpPressed() override;
	virtual void OnExit() override;
	virtual FString GetStateName() override {return "Falling";}

protected:
	virtual void OnLand();
	
private:
	ALedge* DetectNearbyLedge() const;
};