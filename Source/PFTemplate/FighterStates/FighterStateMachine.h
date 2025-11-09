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
	bool TryChangeState(FName NewState);
	void AddState(FName StateName, UFighterState* State);
	void TickCurrentState();
	void ShowStateDebug();

	// Input proxies
	bool Attack() const;
	void AttackReleased() const;
	bool Special() const;
	bool Shield() const;
	void ShieldReleased() const;
	bool Grab() const;
	bool JumpPressed() const; 
	void JumpReleased() const;
	void Parry() const;
};