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

USTRUCT()
struct FStickConfig
{
	FFixed_32 DownThreshold  = FFixed_32(0.5f);
	FFixed_32 FlickStart = FFixed_32(0.4f);
	FFixed_32 FlickEnd = FFixed_32(0.7f);
	FFixed_32 DeadZone = FFixed_32(0.1f);
};

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

	uint16 Down = 0;
	uint16 Pressed = 0;
	FFixedVector2D StickPos;
	FStickConfig StickConfig;

	bool IsHeld(EInputButton Bit) const
	{
		return (Down & static_cast<uint16>(Bit)) != 0;
	}

	bool IsPressed(EInputButton Bit) const
	{
		return (Pressed & static_cast<uint16>(Bit)) != 0;
	}
};

FORCEINLINE void UpdateStickState(FFighterInput& S, FFixedVector2D New, FFixedVector2D Old)
{
	if (New.X.Abs() < S.StickConfig.DeadZone) New.X = FFixed_32(0.f);
	if (New.Z.Abs() < S.StickConfig.DeadZone) New.Z = FFixed_32(0.f);
	
	S.StickPos = New;
	
	if ((S.StickPos.Z < -S.StickConfig.DownThreshold) && !(Old.Z < -S.StickConfig.DownThreshold))
	{
		S.Pressed |= static_cast<uint32>(EInputButton::StickDown);
	}

	auto FlickAxis = [&](FFixed_32 Curr, FFixed_32 Prev)
	{
		const bool bCurrStrong = Curr.Abs() >= S.StickConfig.FlickEnd;
		const bool bPrevNeutral = Prev.Abs() <= S.StickConfig.FlickStart;

		const bool bSignFlip =
			Prev.Abs() >= S.StickConfig.FlickEnd &&
			((Curr > FFixed_32(0)) != (Prev > FFixed_32(0)));

		return bCurrStrong && (bPrevNeutral || bSignFlip);
	};

	bool FlickX = FlickAxis(S.StickPos.X, Old.X);
	bool FlickY = FlickAxis(S.StickPos.Z, Old.Z);
	
	if (FlickX || FlickY) S.Pressed |=static_cast<uint32>(EInputButton::Flick);
}