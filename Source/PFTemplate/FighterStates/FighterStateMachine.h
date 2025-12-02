#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "FighterStateMachine.generated.h"

class AFighterPawn;
class UFighterState;

UCLASS(Blueprintable)
class PFTEMPLATE_API UFighterStateMachine : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY() AFighterPawn* FighterPawnRef;
	UPROPERTY() UFighterState* CurrentState;
	UPROPERTY(BlueprintReadOnly) FName CurrentStateKey = NAME_None;
	UPROPERTY() TMap<FName, UFighterState*> StateMap;
	int FramesInState = 0;
	
	void Initialize(AFighterPawn* InOwner);
	bool TryChangeState(FName NewState, FFighterInput &TransitionInput);
	void AddState(FName StateName, UFighterState* State);
	void TickCurrentState(FFighterInput &Input);
	void ShowStateDebug();
};