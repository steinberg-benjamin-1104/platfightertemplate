#pragma once

#include "SafeMath.h"
#include "StickDirection.generated.h"

UENUM(BlueprintType)
enum class EStickDir : uint8
{
	Center,
	Forward,
	Backward,
	Up,
	Down
};

FORCEINLINE static EStickDir GetStickDirection(const FFixedVector2D& Stick, bool bFacingRight)
{
	const FIXED_32 ZERO = 0.f;
	FIXED_32 X = Stick.X;
	FIXED_32 Y = Stick.Z;
	
	if (X == ZERO && Y == ZERO) return EStickDir::Center;

	if (bFacingRight)
	{
		if (X > ZERO)  return EStickDir::Forward;
		if (X < ZERO)  return EStickDir::Backward;
	}
	else
	{
		if (X < ZERO)  return EStickDir::Forward;
		if (X > ZERO)  return EStickDir::Backward;
	}

	if (Y > ZERO) return EStickDir::Up;
	if (Y < ZERO) return EStickDir::Down;

	return EStickDir::Center;
}