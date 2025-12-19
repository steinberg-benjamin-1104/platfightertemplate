#pragma once
#include "CoreMinimal.h"
#include "StickState.h"
#include "ButtonState.generated.h"

UENUM(BlueprintType, meta = (Bitflags))
enum class EMoveButton : uint8
{
	None    = 0 UMETA(Hidden),

	Attack  = 1 << 0,
	Special = 1 << 1,
	Grab    = 1 << 2,
};
ENUM_CLASS_FLAGS(EMoveButton);

enum class EInputButton : uint16
{
	None = 0,

	// Move buttons
	Attack  = 1 << 0,
	Special = 1 << 1,
	Grab    = 1 << 2,

	// Universal system buttons
	Jump    = 1 << 3,
	Shield  = 1 << 4,
	Parry   = 1 << 5,

	// Macros
	Walk     = 1 << 8,
	Heavy    = 1 << 9,
	Shorthop = 1 << 10,
	Fullhop  = 1 << 11,
};
ENUM_CLASS_FLAGS(EInputButton);

USTRUCT()
struct FButtonState
{
	GENERATED_BODY()
	
	uint16 Down = 0;
	uint16 Pressed = 0;

	FORCEINLINE bool IsHeld(EInputButton Bit) const
	{
		return (Down & static_cast<uint16>(Bit)) != 0;
	}

	FORCEINLINE bool IsPressed(EInputButton Bit) const
	{
		return (Pressed & static_cast<uint16>(Bit)) != 0;
	}
	
	FORCEINLINE void ProcessPressed()
	{
		Down |= Pressed;
		Pressed = 0;
	}
};

static uint16 CompileMoveButtonMask(EMoveButton Buttons)
{
	uint16 Mask = 0;

	if (EnumHasAnyFlags(Buttons, EMoveButton::Attack))
		Mask |= static_cast<uint16>(EInputButton::Attack);

	if (EnumHasAnyFlags(Buttons, EMoveButton::Special))
		Mask |= static_cast<uint16>(EInputButton::Special);

	if (EnumHasAnyFlags(Buttons, EMoveButton::Grab))
		Mask |= static_cast<uint16>(EInputButton::Grab);

	return Mask;
}