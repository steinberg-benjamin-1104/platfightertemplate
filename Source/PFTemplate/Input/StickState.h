#pragma once
#include "CoreMinimal.h"
#include "SafeMath.h"
#include "StickState.generated.h"

USTRUCT()
struct FStickState
{
	// Raw input
	FFixedVector2D Current = FFixedVector2D();
	FFixedVector2D Previous = FFixedVector2D();

	// Derived states
	bool bDownThisFrame = false;        // Y crossing threshold
	bool bLeftThisFrame = false;
	bool bRightThisFrame = false;
	bool bUpThisFrame = false;

	bool bFlick = false;                // Single-frame flick detection
	FFixedVector2D FlickDirection = FFixedVector2D();

	bool bWalking = false;              // |X| ≤ WalkRadius

	// Config (tune for your engine)
	FIXED_32 DownThreshold  = FIXED_32(0.5f);
	FIXED_32 FlickThreshold = FIXED_32(0.7f);
	FIXED_32 WalkRadius     = FIXED_32(0.4f);

	void PrepareForNextFrame(FFixedVector2D New)
	{
		Previous = Current;
		Current = New;
	}
};

void UpdateStickState(FStickState& Stick, FFixedVector2D StickInput);