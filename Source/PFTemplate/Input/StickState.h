#pragma once
#include "CoreMinimal.h"
#include "SafeMath.h"
#include "StickState.generated.h"

USTRUCT()
struct FStickState
{
	GENERATED_BODY()
	
	FFixedVector2D StickPos = FFixedVector2D();
	
	bool bDownThisFrame = false;        
	bool bFlick = false;                
	
	// Config
	FFixed_32 DownThreshold  = FFixed_32(0.5f);
	FFixed_32 FlickStart = FFixed_32(0.4f);
	FFixed_32 FlickEnd = FFixed_32(0.7f);

	// Flick buffer (added)
	FFixedVector2D FlickPos = FFixedVector2D();
	int32 FlickBufferFrames = 0;
	int32 MaxFlickBufferFrames = 5;

	void ConsumeFlick() { FlickBufferFrames = 0; }
};

inline void UpdateStickState(FStickState& S, FFixedVector2D New, FFixedVector2D Old)
{
	S.StickPos = New;
	
	S.bDownThisFrame = false;
	if ((S.StickPos.Z < -S.DownThreshold) && !(Old.Z < -S.DownThreshold))
	{
		S.bDownThisFrame = true;
	}

	auto FlickAxis = [&](FFixed_32 Curr, FFixed_32 Prev)
	{
		const bool bCurrStrong = Curr.Abs() >= S.FlickEnd;
		const bool bPrevNeutral = Prev.Abs() <= S.FlickStart;

		const bool bSignFlip =
			Prev.Abs() >= S.FlickEnd &&
			((Curr > FFixed_32(0)) != (Prev > FFixed_32(0)));

		return bCurrStrong && (bPrevNeutral || bSignFlip);
	};

	bool FlickX = FlickAxis(S.StickPos.X, Old.X);
	bool FlickY = FlickAxis(S.StickPos.Z, Old.Z);

	if (FlickX || FlickY)
	{
		S.FlickBufferFrames = S.MaxFlickBufferFrames;
		S.bFlick = true;
		S.FlickPos = S.StickPos;
	}
	else if (S.FlickBufferFrames > 0)
	{
		S.FlickBufferFrames--;
	}
	else
	{
		S.bFlick = false;
	}
}