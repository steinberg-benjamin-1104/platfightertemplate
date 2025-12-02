#pragma once
#include "CoreMinimal.h"
#include "StickState.h"
#include "ButtonState.h"
#include "FighterInput.generated.h"

USTRUCT()
struct FFighterInput
{
	GENERATED_BODY()
	FStickState Stick;
	FButtonState Button;
};