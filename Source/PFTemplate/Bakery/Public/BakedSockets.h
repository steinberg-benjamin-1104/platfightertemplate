#pragma once

#include "BakedSockets.generated.h"

USTRUCT(BlueprintType)
struct FBakedSocketKey
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D Location2D = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RotationAngle = 0.0f;
};

USTRUCT(BlueprintType)
struct FBakedSocketTrack
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FName SocketName;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FBakedSocketKey> Frames;
};