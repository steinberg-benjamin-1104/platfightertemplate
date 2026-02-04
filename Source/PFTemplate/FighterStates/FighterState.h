#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "InputBuffer.h"
#include "StickDirection.h"
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
	
	FInputBuffer* InputBuffer;

	virtual FString GetStateName() {return "StateName";}

	virtual void InitState(
		AFighterPawn* InFighterPawn,
		UFighterMovementComponent* InMoveComp,
		UFighterStateMachine* InStateMachine,
		FInputBuffer* InInputBuffer);
	
	virtual void OnEnter() {}
	virtual void OnExit() {}
	virtual bool CanEnterState() const { return true; }
	virtual bool CanExitState() const { return true; }
	virtual void HandleInput() { }
	virtual bool HandlePhysics() { return false;}
	virtual bool HandleTimer(int32 FramesInState) { return false;}

	virtual void Tick(int32 FramesInState)
	{
		if (HandleTimer(FramesInState)) return;
		if (HandlePhysics()) return;
		HandleInput();
	}

protected:
	EStickDir GetCurrentStickDir();
};