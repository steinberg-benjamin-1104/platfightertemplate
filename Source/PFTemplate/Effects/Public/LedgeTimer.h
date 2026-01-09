#pragma once

#include "CoreMinimal.h"
#include "EffectBase.h"
#include "FighterPawn.h"
#include "LedgeTimer.generated.h"

UCLASS()
class PFTEMPLATE_API ULedgeTimer : public UEffectBase
{
	GENERATED_BODY()
public:
	
	ULedgeTimer()
	{
		Duration = 180;
	}
	
	void OnEnter() override { FighterPawnRef->bCanLedge = false; }
	void OnExit() override { FighterPawnRef->bCanLedge = true; }
};