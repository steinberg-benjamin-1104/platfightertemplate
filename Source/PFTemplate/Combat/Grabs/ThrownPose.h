#pragma once

#include "CoreMinimal.h"
#include "ThrownPose.generated.h"

UENUM(BlueprintType)
enum class FThrownPose : uint8
{
	
	Upright		UMETA(DisplayName = "Upright"),
	FaceUp		UMETA(DisplayName = "Face Up"),
	FaceDown	UMETA(DisplayName = "Face Down"),
	
};