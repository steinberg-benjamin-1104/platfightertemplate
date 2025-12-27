#pragma once
#include "Engine/DataTable.h"
#include "StickDirection.h"
#include "AttackDefinition.generated.h"

USTRUCT(BlueprintType)
struct FAttackDefinition : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAnimation Animation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> ValidStates;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bFlickInput = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMoveButton MoveButton = EMoveButton::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EStickDir StickDir = EStickDir::Center;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName TargetState = NAME_None;
};