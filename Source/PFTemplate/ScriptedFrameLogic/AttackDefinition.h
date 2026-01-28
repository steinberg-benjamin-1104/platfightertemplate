#pragma once
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FName> ValidStates;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bFlickInput = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EMoveButton MoveButton = EMoveButton::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EStickDir StickDir = EStickDir::Center;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsSpecial = false;

	UPROPERTY(EditAnywhere)
	TArray<FHitboxGroup> HitboxGroups;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FAttackWindow> Windows;
};