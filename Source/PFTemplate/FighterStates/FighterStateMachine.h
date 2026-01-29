#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "FighterStateMachine.generated.h"

USTRUCT()
struct FFSMSnapshot
{
	GENERATED_BODY()

	int32 FramesInState = -1;
	FName CurrentStateKey;
};

class AFighterPawn;
class UFighterState;

UCLASS(Blueprintable)
class PFTEMPLATE_API UFighterStateMachine : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY() FFSMSnapshot FSMSnapshot;
	UPROPERTY() UFighterState* CurrentState;
	UPROPERTY() TMap<FName, UFighterState*> StateMap;
	
	void Initialize(AFighterPawn* InOwner);
	void ChangeFighterState(FName NewState);
	void AddState(FName StateName, UFighterState* State);
	void TickCurrentState();
	void ShowStateDebug();

	//Snapshot
	FName GetCurrentStateKey() const { return FSMSnapshot.CurrentStateKey; }
	int32 GetFrame() const { return FSMSnapshot.FramesInState; }

private:
	void ResetFrameCount() { FSMSnapshot.FramesInState = -1; }
	void IncrementFrame() { FSMSnapshot.FramesInState++; }
	void SetCurrentStateKey(FName NewKey) { FSMSnapshot.CurrentStateKey = NewKey; }
	
};