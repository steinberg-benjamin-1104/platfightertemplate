#pragma once
#include "CoreMinimal.h"
#include "StickState.h"
#include "ButtonState.generated.h"

UENUM()
enum class EInputButton : uint8
{
	None    = 0      UMETA(Hidden),
	Attack  = 1 << 0,
	Special = 1 << 1,
	Shield  = 1 << 2,
	Jump    = 1 << 3,
	Parry   = 1 << 4,
	Grab    = 1 << 5,
};
ENUM_CLASS_FLAGS(EInputButton);

UENUM()
enum class EInputMacros : uint8
{
	None     = 0     UMETA(Hidden),
	Walk     = 1 << 0,
	Heavy    = 1 << 1,
	Shorthop = 1 << 2,
	Fullhop  = 1 << 3,
};
ENUM_CLASS_FLAGS(EInputMacros);

USTRUCT()
struct FButtonState
{
	GENERATED_BODY()
	EInputButton ButtonsPressed = EInputButton::None;
	EInputButton ButtonsHeld = EInputButton::None;
	EInputMacros MacrosPressed = EInputMacros::None;

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