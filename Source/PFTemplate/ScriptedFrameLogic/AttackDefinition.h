#pragma once
#include "Engine/DataTable.h"
#include "ButtonState.h"
#include "StickDirection.h"
#include "AttackDefinition.generated.h"

USTRUCT(BlueprintType)
struct FAttackDefinition : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FDataTableRowHandle AnimationRow;
	
	TArray<FName> ValidStates;
	bool bFlickInput = false;
	EInputButton InputButton = EInputButton::None;
	EStickDir StickDir = EStickDir::Center;
	FName TargetState = NAME_None;
};