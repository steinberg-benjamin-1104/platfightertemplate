#pragma once
#include "CoreMinimal.h"
#include "FighterInput.h"
#include "InputBuffer.generated.h"

USTRUCT()
struct FInputBuffer
{
	GENERATED_BODY()

	static constexpr int32 SIZE   = 5;
	static constexpr int32 WINDOW = 5;

	UPROPERTY()
	TArray<FFighterInput> Ring;

	UPROPERTY()
	int32 WriteIdx = 0;

	UPROPERTY()
	int32 SimFrame = 0;
	
	UPROPERTY()
	int32 FrameAtIndex[SIZE] = { 0 };

	
	void Push(const FFighterInput& I)
	{
		if (Ring.Num() < SIZE)
			Ring.AddDefaulted();

		Ring[WriteIdx] = I;
		FrameAtIndex[WriteIdx] = SimFrame;

		WriteIdx = (WriteIdx + 1) % SIZE;
	}
	
	bool Consume(FFighterInput::Button B, FFighterInput& Out)
	{
		for (int32 i = 0; i < Ring.Num(); ++i)
		{
			int32 idx = (WriteIdx - 1 - i + SIZE) % SIZE;
			
			if (SimFrame - FrameAtIndex[idx] > WINDOW)
				continue;

			if (Ring[idx].Pressed(B))
			{
				Out = Ring[idx];
				Ring[idx].Buttons &= ~B;
				return true;
			}
		}

		return false;
	}
};