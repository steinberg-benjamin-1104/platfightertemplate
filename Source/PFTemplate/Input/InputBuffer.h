#pragma once
#include "CoreMinimal.h"
#include "FighterInput.h"
#include "InputBuffer.generated.h"

USTRUCT()
struct FInputBuffer
{
	GENERATED_BODY()
    
	static const int32 BUFFER_SIZE = 5; 
	FFighterInput History[BUFFER_SIZE];
	int32 CurrentFrameIndex = 0;

	void Update(const FFighterInput& NewInput)
	{
		CurrentFrameIndex = (CurrentFrameIndex + 1) % BUFFER_SIZE;
		History[CurrentFrameIndex] = NewInput;
	}

	FFighterInput* WasPressed(EInputButton Button, int32 Window = 6)
	{
		for (int32 i = 0; i < Window; ++i)
		{
			int32 CheckIndex = (CurrentFrameIndex - i + BUFFER_SIZE) % BUFFER_SIZE;
			
			if (History[CheckIndex].IsAvailable(Button))
			{
				return &History[CheckIndex];
			}
		}
		return nullptr;
	}

	bool IsHeld(EInputButton Button) {return History[CurrentFrameIndex].IsHeld(Button); }

	FFighterInput* GetRecent() { return &History[CurrentFrameIndex]; }
};