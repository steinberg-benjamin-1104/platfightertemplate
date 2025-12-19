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
	const FFixed_32 ZERO = FFixed_32(0);

	const FFixed_32 X = Stick.X;
	const FFixed_32 Y = Stick.Z;
	
	if (X == ZERO && Y == ZERO)
	{
		return EStickDir::Center;
	}
	
	if (X != ZERO)
	{
		if (bFacingRight)
		{
			return (X > ZERO) ? EStickDir::Forward : EStickDir::Backward;
		}
		else
		{
			return (X < ZERO) ? EStickDir::Forward : EStickDir::Backward;
		}
	}

	// Vertical only
	return (Y > ZERO) ? EStickDir::Up : EStickDir::Down;
}
