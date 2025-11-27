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
	bool bDownThisFrame = false;        
	bool bLeftThisFrame = false;
	bool bRightThisFrame = false;
	bool bUpThisFrame = false;

	bool bFlick = false;                
	FFixedVector2D FlickPosition = FFixedVector2D();

	bool bWalking = false;              

	// Config
	FIXED_32 DownThreshold  = FIXED_32(0.5f);
	FIXED_32 FlickThreshold = FIXED_32(0.7f);
	FIXED_32 WalkRadius     = FIXED_32(0.4f);

	// Flick buffer (added)
	int32 FlickBufferFrames = 0;
	int32 MaxFlickBufferFrames = 5;

	void PrepareForNextFrame(FFixedVector2D New)
	{
		Previous = Current;
		Current = New;
	}
};

inline void UpdateStickState(FStickState& S, FFixedVector2D NewStickInput)
{
	S.PrepareForNextFrame(NewStickInput);

	S.bDownThisFrame  = (S.Current.Z < -S.DownThreshold) && !(S.Previous.Z < -S.DownThreshold);
	S.bUpThisFrame    = (S.Current.Z >  S.DownThreshold) && !(S.Previous.Z >  S.DownThreshold);

	S.bLeftThisFrame  = (S.Current.X < -S.DownThreshold) && !(S.Previous.X < -S.DownThreshold);
	S.bRightThisFrame = (S.Current.X >  S.DownThreshold) && !(S.Previous.X >  S.DownThreshold);

	bool FlickX = (S.Current.X.Abs() > S.FlickThreshold && S.Previous.X.Abs() < S.FlickThreshold);
	bool FlickY = (S.Current.Z.Abs() > S.FlickThreshold && S.Previous.Z.Abs() < S.FlickThreshold);
	
	if (FlickX || FlickY)
	{
		S.FlickBufferFrames = S.MaxFlickBufferFrames;
		S.bFlick = true;
		S.FlickPosition = S.Current;
	}
	else
	{
		if (S.FlickBufferFrames > 0)
		{
			S.FlickBufferFrames--;
			S.bFlick = true;
		}
		else
		{
			S.bFlick = false;
			S.FlickPosition = FFixedVector2D();
		}
	}

	S.bWalking = (S.Current.X.Abs() <= S.WalkRadius);
}