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
	UPROPERTY() UFighterState* CurrentState;
	UPROPERTY(BlueprintReadOnly) FName CurrentStateKey = NAME_None;
	UPROPERTY() TMap<FName, UFighterState*> StateMap;
	int FramesInState = 0;
	
	void Initialize(AFighterPawn* InOwner);
	void ChangeFighterState(FName NewState);
	void AddState(FName StateName, UFighterState* State);
	void TickCurrentState();
	void ShowStateDebug();
};