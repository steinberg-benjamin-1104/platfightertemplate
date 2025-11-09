#pragma once
#include "CoreMinimal.h"
#include "BoneVectorAxis.generated.h"

UENUM(BlueprintType)
enum class EBoneVectorAxis : uint8
{
	Forward UMETA(DisplayName = "Forward"),
	Right UMETA(DisplayName = "Right"),
	Up UMETA(DisplayName = "Up")
};