#pragma once
#include "CoreMinimal.h"
#include "EffectBase.h"
#include "EffectMachine.generated.h"

class AFighterPawn;
class UEffectBase;

struct FEffectSnapshot
{
	FEffectSnapshot(FName ID, int32 InDuration)
	{
		EffectID = ID;
		Duration = InDuration;
	}
	
	FName EffectID;
	int32 Duration;

	bool Update()
	{
		Duration--;
		return Duration == 0;
	}
};

struct FEffectMachineSnapshot
{
	TArray<FEffectSnapshot> ActiveEffects;

	void AddEffect(const FEffectSnapshot NewEffect) { ActiveEffects.AddUnique(NewEffect); }
	void RemoveEffect(FEffectSnapshot Effect) { ActiveEffects.Remove(Effect); }
	int32 GetNum() { return ActiveEffects.Num(); }
	FName GetEffect(int32 i) { return ActiveEffects[i].EffectID; }
};

UCLASS()
class PFTEMPLATE_API UEffectMachine : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(AFighterPawn* InOwner);
	void UpdateEffects();
	void EnableEffect(FName NewEffect, int32 InDuration = 0);

protected:
	UPROPERTY() AFighterPawn* FighterPawnRef = nullptr;

private:
	UPROPERTY() FEffectMachineSnapshot EMSnapshot;
	UPROPERTY() TMap<FName, UEffectBase*> EffectMap;
};