#pragma once
#include "Animation.h"
#include "Engine/DataTable.h"
#include "ButtonState.h"
#include "AttackDefinition.generated.h"

USTRUCT(BlueprintType)
struct FAttackDefinition : public FTableRowBase
{
	FAnimation Animation;
	TArray<FName> ValidStates;
	bool bFlickInput = false;
	EInputButton InputButton = EInputButton::None;
	//stick input direction
	FName TargetState = NAME_None;
	bool bGroundOnly = true;
};