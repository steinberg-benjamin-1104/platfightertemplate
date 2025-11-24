#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ButtonState.h"
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

	UFUNCTION(BlueprintCallable) virtual FString GetStateName() {return "StateName";}

	virtual void InitState(AFighterPawn* InFighterPawn, UFighterMovementComponent* InMoveComp, UFighterStateMachine* InStateMachine)
	{
		FighterPawnRef = InFighterPawn;
		MoveComp = InMoveComp;
		StateMachine = InStateMachine;
	}
	
	UFUNCTION(BlueprintCallable) virtual void OnEnter() {}
	UFUNCTION(BlueprintCallable) virtual void OnExit() {}
	UFUNCTION(BlueprintCallable) virtual bool CanEnterState() const { return true; }
	UFUNCTION(BlueprintCallable) virtual bool CanExitState() const { return true; }
	UFUNCTION(BlueprintCallable) virtual bool HandleButtonInput(FFighterInput &Input) {}
	UFUNCTION(BlueprintCallable) virtual bool HandleStickInput(FFighterInput &Input) {}
	UFUNCTION(BlueprintCallable) virtual bool HandlePhysics(FFighterInput &Input) {}
	UFUNCTION(BlueprintCallable) virtual void Tick(FFighterInput &Input)
	{
		if (HandleButtonInput(Input)) return;
		if (HandleStickInput(Input)) return;
		if (HandlePhysics()) return;
	}
};