#pragma once

#include "FighterMovementMode.generated.h"

UENUM(BlueprintType)
enum class EFighterMovementMode : uint8
{
	Grounded UMETA(DisplayName = "Grounded"),
	JumpingUp UMETA(DisplayName = "Jumping Up"),
	Falling UMETA(DisplayName = "Falling"),
	KeepCurrent UMETA(DisplayName = "Keep Current"),
	AutoResolve UMETA(DisplayName = "Auto Resolve"),
	Custom UMETA(DisplayName = "None")
};