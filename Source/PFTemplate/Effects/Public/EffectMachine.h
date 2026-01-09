#pragma once
#include "CoreMinimal.h"
#include "EffectMachine.generated.h"

class AFighterPawn;
class UEffectBase;

UCLASS()
class PFTEMPLATE_API UEffectMachine : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(AFighterPawn* InOwner);
	void UpdateEffectTimers();
	void UpdateActiveEffects();
	void EnableEffect(FName NewEffect, int32 InDuration = 0);

protected:
	UPROPERTY() AFighterPawn* FighterPawnRef = nullptr;

private:
	UPROPERTY() TArray<UEffectBase*> ActiveEffects;
	UPROPERTY() TMap<FName, UEffectBase*> EffectMap;
};