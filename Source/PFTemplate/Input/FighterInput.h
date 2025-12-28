#pragma once
#include "CoreMinimal.h"
#include "SafeMath.h"
#include "FighterInput.generated.h"

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

	// Stick
	StickDown = 1 << 6,
	Flick = 1 << 7,

	// Macros
	Heavy    = 1 << 8,
	Shorthop = 1 << 9,
	Fullhop  = 1 << 10,
};
ENUM_CLASS_FLAGS(EInputButton);

USTRUCT()
struct FFighterInput
{
	GENERATED_BODY()
	
	uint16 Pressed = 0;
	uint16 Held = 0;
	uint16 Consumed = 0;
	FFixedVector2D StickPos;

	bool IsHeld(EInputButton Bit) const
	{
		return (Held & static_cast<uint16>(Bit)) != 0;
	}

	bool IsPressed(EInputButton Bit) const
	{
		return (Pressed & static_cast<uint16>(Bit)) != 0;
	}

	bool IsAvailable(EInputButton Bit) const
	{
		uint16 Mask = static_cast<uint16>(Bit);
		return (Pressed & Mask) != 0 && (Consumed & Mask) == 0;
	}

	void Consume(EInputButton Button)
	{
		Consumed |= static_cast<uint16>(Button);
		Held |= static_cast<uint16>(Button);
	}
};

FORCEINLINE uint16 CompileMoveButtonMask(EMoveButton MoveButton)
{
	uint16 Mask = 0;
	
	if (!!(MoveButton & EMoveButton::Attack))
		Mask |= static_cast<uint16>(EInputButton::Attack);

	if (!!(MoveButton & EMoveButton::Special))
		Mask |= static_cast<uint16>(EInputButton::Special);

	if (!!(MoveButton & EMoveButton::Grab))
		Mask |= static_cast<uint16>(EInputButton::Grab);

	return Mask;
}