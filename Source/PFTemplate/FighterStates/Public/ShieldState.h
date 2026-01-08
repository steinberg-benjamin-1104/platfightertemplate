#pragma once

#include "CoreMinimal.h"
#include "FighterState.h"
#include "ShieldState.generated.h"

class UShieldComponent;
UCLASS()
class PFTEMPLATE_API UShieldState : public UFighterState
{
	GENERATED_BODY()

public:

	virtual void InitState(AFighterPawn* InFighterPawn, UFighterMovementComponent* InMoveComp, UFighterStateMachine* InStateMachine, FInputBuffer* InBuffer) override;
	virtual void OnEnter() override;
	virtual void HandleInput() override;
	virtual bool HandlePhysics() override;
	virtual void OnExit() override;

	virtual FString GetStateName() override {return "Shield";}

private:
	UPROPERTY() UShieldComponent* ShieldComponent;
};