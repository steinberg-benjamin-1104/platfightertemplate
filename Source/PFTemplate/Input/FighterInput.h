#pragma once
#include "CoreMinimal.h"
#include "SafeMath.h"
#include "FighterInput.generated.h"

USTRUCT()
struct FFighterInput
{
	GENERATED_BODY()

	UPROPERTY()
	FFixedVector2D Stick{ FIXED_32(0.f), FIXED_32(0.f) };

	enum Button : uint8
	{
		Attack = 1<<0,
		Special= 1<<1,
		Shield = 1<<2,
		Grab   = 1<<3,
		Jump   = 1<<4,
		Parry  = 1<<5,
		Walk   = 1<<6,
	};

	UPROPERTY()
	uint8 Buttons = 0;

	// simple bit helpers
	bool Pressed (Button B) const { return (Buttons & B) != 0; }
	bool Released(Button B) const { return (Buttons & B) == 0; }

	bool operator==(const FFighterInput& O) const
	{
		return Stick.X == O.Stick.X && Stick.Z == O.Stick.Z && Buttons == O.Buttons;
	}

	friend uint32 GetTypeHash(const FFighterInput& I)
	{
	    uint32 H = 0;
	    HashCombineFast(H, GetTypeHash(I.Stick.X));
	    HashCombineFast(H, GetTypeHash(I.Stick.Z));
	    HashCombineFast(H, GetTypeHash(I.Buttons));
	    return H;
	}
};