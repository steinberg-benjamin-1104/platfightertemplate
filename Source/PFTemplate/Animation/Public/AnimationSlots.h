#pragma once
#include "CoreMinimal.h"
#include "BakedSockets.h"
#include "AnimationSlots.generated.h"

UENUM(BlueprintType)
enum class EAnimSlot : uint8
{
	Idle,
	Dash,
	Run,
	Jump,
	Fall,
	Land,
	Hitstun,
	COUNT UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct FCharacterAnimEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBakedAnimation> Animation;

	UPROPERTY(EditAnywhere)
	bool bLoop = true;
};

UCLASS(BlueprintType)
class PFTEMPLATE_API UCharacterAnimationSet : public UDataAsset
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, Category = "Animations")
	TMap<EAnimSlot, FCharacterAnimEntry> EditorAnimations;
	
	UPROPERTY(Transient)
	TArray<FCharacterAnimEntry> BakedAnimations;
	
	FCharacterAnimEntry GetAnimation(EAnimSlot Slot) const
	{
		uint8 Index = static_cast<uint8>(Slot);
		return BakedAnimations.IsValidIndex(Index) ? BakedAnimations[Index] : FCharacterAnimEntry();
	}
	
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override
	{
		Super::PostEditChangeProperty(PropertyChangedEvent);
		BakeAnimations();
	}

	virtual void PostLoad() override
	{
		Super::PostLoad();
		BakeAnimations();
	}

private:
	void BakeAnimations()
	{
		BakedAnimations.SetNumUninitialized(static_cast<int32>(EAnimSlot::COUNT));
		
		for (uint8 i = 0; i < static_cast<uint8>(EAnimSlot::COUNT); ++i)
		{
			EAnimSlot Slot = static_cast<EAnimSlot>(i);
			if (FCharacterAnimEntry* Found = EditorAnimations.Find(Slot))
			{
				BakedAnimations[i] = *Found;
			}
			else
			{
				BakedAnimations[i] = FCharacterAnimEntry(); // Default empty
			}
		}
	}
};