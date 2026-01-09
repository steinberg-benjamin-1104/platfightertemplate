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
	virtual void OnEnter() {}

	void SetDuration(int32 InDuration)
	{
		if (InDuration > 0) Duration = InDuration;
		RemainingFrames = Duration;
	}
	
	virtual bool UpdateFrame()
	{
		if (RemainingFrames <= 0) return true;
		RemainingFrames--;
		return false;
	}
	
	virtual void Tick() {}
	virtual void OnExit() {}

protected:
	UPROPERTY() AFighterPawn* FighterPawnRef = nullptr;

	int32 Duration = 60;
	int32 RemainingFrames = 0;
	
};