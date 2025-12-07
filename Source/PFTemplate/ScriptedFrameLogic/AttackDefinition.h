#pragma once
#include "Engine/DataTable.h"
#include "ButtonState.h"
#include "StickDirection.h"
#include "AttackDefinition.generated.h"

USTRUCT(BlueprintType)
struct FAttackDefinition : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDataTableRowHandle AnimationRow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> ValidStates;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bFlickInput = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EInputButton InputButton = EInputButton::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EStickDir StickDir = EStickDir::Center;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName TargetState = NAME_None;
};