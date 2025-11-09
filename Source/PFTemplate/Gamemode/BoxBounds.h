#pragma once

#include "CoreMinimal.h"
#include "BoxBounds.generated.h"

USTRUCT(BlueprintType)
struct FBoxBounds
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage")
	float Left = -4000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage")
	float Right = 4000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage")
	float Top = 3000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage")
	float Bottom = -2000.f;

	FBoxBounds()
		: Left(0), Right(0), Top(0), Bottom(0) {}

	FBoxBounds(float InLeft, float InRight, float InBottom, float InTop)
		: Left(InLeft), Right(InRight), Top(InTop), Bottom(InBottom) {}
};