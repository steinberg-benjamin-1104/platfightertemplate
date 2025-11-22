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
	Parry   = 1 << 4,
	Grab    = 1 << 5,
};
ENUM_CLASS_FLAGS(EInputButton);

USTRUCT()
struct FFighterInput
{
	EInputButton ButtonsPressed = EInputButton::None;
	EInputButton ButtonsHeld = EInputButton::None;  

	FFixedVector2D LeftStick = FFixedVector2D();

	FFighterInput ClearPressed() const
	{
		FFighterInput New = *this;
		New.ButtonsPressed = EInputButton::None;
		New.ButtonsHeld |= ButtonsPressed;
		return New;
	}

	bool IsPressed(EInputButton Button) const
	{
		return EnumHasAnyFlags(ButtonsPressed, Button);
	}

	bool IsHeld(EInputButton Button) const
	{
		return EnumHasAnyFlags(ButtonsHeld, Button);
	}
};