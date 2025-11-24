#pragma once
#include "CoreMinimal.h"
#include "StickState.h"
#include "ButtonState.generated.h"

UENUM(BlueprintType)
enum class EInputButton : uint16
{
	None    = 0        UMETA(Hidden),
	Attack  = 1 << 0,
	Special = 1 << 1,
	Shield  = 1 << 2,
	Jump    = 1 << 3,
	Parry   = 1 << 4,
	Grab    = 1 << 5,
	Walk    = 1 << 6,
	Heavy   = 1 << 7,
	Shorthop= 1 << 8,
	Fullhop = 1 << 9,
};

ENUM_CLASS_FLAGS(EInputButton);

USTRUCT()
struct FButtonState
{
	EInputButton ButtonsPressed = EInputButton::None;
	EInputButton ButtonsHeld = EInputButton::None;  

	FButtonState ClearPressed() const
	{
		FButtonState New = *this;
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