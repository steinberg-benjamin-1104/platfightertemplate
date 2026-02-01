#pragma once
#include "BakedAnimation.h"
#include "StickDirection.h"
#include "BakedSockets.h"
#include "Windows.h"
#include "AttackDefinition.generated.h"

UCLASS(BlueprintType)
class PFTEMPLATE_API UAttackDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UBakedAnimation* BakedAnim;

	//check for b-reverse/wavebounce
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsSpecial = false;

	UPROPERTY(EditAnywhere)
	TArray<FHitboxCollection> HitboxCollections;

	//For cancels, either on-hit or just open windows
	//also custom movement data
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FAttackWindow> Windows;
};