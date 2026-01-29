#pragma once
#include "CoreMinimal.h"
#include "EffectBase.generated.h"

class AFighterPawn;

UCLASS()
class PFTEMPLATE_API UEffectBase : public UObject
{
	GENERATED_BODY()

public:
	virtual void Initialize(AFighterPawn* InOwner) { FighterPawnRef = InOwner; }
	virtual void OnEnter() { }
	
	virtual void Tick() {}
	virtual void OnExit() {}

protected:
	UPROPERTY() AFighterPawn* FighterPawnRef = nullptr;
	
};