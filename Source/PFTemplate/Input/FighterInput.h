#pragma once
#include "CoreMinimal.h"
#include "StickState.h"
#include "ButtonState.h"
#include "FighterInput.generated.h"

USTRUCT()
struct FFighterInput
{
	FStickState Stick;
	FButtonState Button;
};