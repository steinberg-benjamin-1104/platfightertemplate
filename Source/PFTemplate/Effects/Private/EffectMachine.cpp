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

	UEffectBase* Effect = EffectMap[NewEffect];

	Effect->SetDuration(InDuration);
	Effect->OnEnter();
	ActiveEffects.AddUnique(Effect);
}

void UEffectMachine::UpdateEffectTimers()
{
	for (int32 i = ActiveEffects.Num() - 1; i >= 0; --i)
	{
		UEffectBase* Effect = ActiveEffects[i];
		
		if (Effect->UpdateFrame())
		{
			Effect->OnExit();
			ActiveEffects.RemoveAt(i);
		}
	}
}

void UEffectMachine::UpdateActiveEffects()
{
	for (UEffectBase* Effect : ActiveEffects) Effect->Tick(); 
}