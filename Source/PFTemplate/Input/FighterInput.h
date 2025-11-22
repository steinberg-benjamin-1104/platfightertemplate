#pragma once
#include "CoreMinimal.h"
#include "SafeMath.h"
#include "FighterInput.generated.h"

UENUM(BlueprintType)
enum class EInputButton : uint8
{
	None    = 0        UMETA(Hidden),
	Attack  = 1 << 0,
	Special = 1 << 1,
	Shield  = 1 << 2,
	Jump    = 1 << 3,
};
ENUM_CLASS_FLAGS(EInputButton);

struct FFighterInput
{
	EInputButton ButtonsDown = EInputButton::None;  
	EInputButton ButtonsPressed = EInputButton::None;

	FFixedVector2D LeftStick = FFixedVector2D();
};