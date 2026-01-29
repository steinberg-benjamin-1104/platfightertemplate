#include "EffectMachine.h"
#include "LedgeTimer.h"

void UEffectMachine::Initialize(AFighterPawn* InOwner)
{
	FighterPawnRef = InOwner;

	EffectMap.Add("LedgeTimer", NewObject<ULedgeTimer>(this));

	for (auto& Elem : EffectMap)
	{
		if (Elem.Value) Elem.Value->Initialize(InOwner);
	}
}

void UEffectMachine::EnableEffect(FName NewEffect, int32 InDuration)
{
	if (!EffectMap.Contains(NewEffect)) return;

	EffectMap[NewEffect]->OnEnter();
	EMSnapshot.AddEffect(FEffectSnapshot(NewEffect, InDuration));
}

void UEffectMachine::UpdateEffects()
{
	for (FEffectSnapshot EffectSnapshot : EMSnapshot.ActiveEffects)
	{
		UEffectBase* Effect = EffectMap[EffectSnapshot.EffectID];
		
		if (EffectSnapshot.Update())
		{
			Effect->OnExit();
			EMSnapshot.RemoveEffect(EffectSnapshot);
		}
		else Effect->Tick();
	}
}