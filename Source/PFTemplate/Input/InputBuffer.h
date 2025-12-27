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
	
	uint16 ConsumedMask = 0;

	void Update(const FFighterInput& NewInput)
	{
		CurrentFrameIndex = (CurrentFrameIndex + 1) % BUFFER_SIZE;
		History[CurrentFrameIndex] = NewInput;
		ConsumedMask = 0; 
	}

	FFighterInput* WasPressed(EInputButton Button, int32 Window = 6)
	{
		if ((ConsumedMask & static_cast<uint16>(Button)) != 0) return false;
		
		for (int32 i = 0; i < Window; ++i)
		{
			int32 CheckIndex = (CurrentFrameIndex - i + BUFFER_SIZE) % BUFFER_SIZE;
			if (History[CheckIndex].IsPressed(Button))
			{
				return &History[CheckIndex];
			}
		}
		return nullptr;
	}
	
	void Consume(EInputButton Button)
	{
		ConsumedMask |= static_cast<uint16>(Button);
	}

	bool IsHeld(EInputButton Button) {return History[CurrentFrameIndex].IsHeld(Button); }
};