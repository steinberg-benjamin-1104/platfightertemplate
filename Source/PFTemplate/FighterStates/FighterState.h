#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FighterInput.h"
#include "FighterState.generated.h"

class AFighterPawn;
class UFighterMovementComponent;
class UFighterStateMachine;

UCLASS(Abstract, Blueprintable)
class PFTEMPLATE_API UFighterState : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	AFighterPawn* FighterPawnRef;

	UPROPERTY()
	UFighterMovementComponent* MoveComp;

	UPROPERTY()
	UFighterStateMachine* StateMachine;

	virtual FString GetStateName() {return "StateName";}

	virtual void InitState(AFighterPawn* InFighterPawn, UFighterMovementComponent* InMoveComp, UFighterStateMachine* InStateMachine)
	{
		FighterPawnRef = InFighterPawn;
		MoveComp = InMoveComp;
		StateMachine = InStateMachine;
	}
	
	virtual void OnEnter(FFighterInput &Input) {}
	virtual void OnExit() {}
	virtual bool CanEnterState() const { return true; }
	virtual bool CanExitState() const { return true; }
	virtual bool HandleButtonInput(FFighterInput &Input) { return false; }
	virtual bool HandleStickInput(FFighterInput &Input) { return false; }
	virtual bool HandlePhysics(FFighterInput &Input) { return false;}
	virtual bool HandleTimer(FFighterInput &Input, int32 FramesInState) { return false;}

	virtual void Tick(FFighterInput &Input, int32 FramesInState)
	{
		if (HandleTimer(Input, FramesInState)) return;
		if (HandlePhysics(Input)) return;
		if (HandleButtonInput(Input)) return;
		if (HandleStickInput(Input)) return;
	}
};