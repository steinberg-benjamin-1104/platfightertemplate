#pragma once
#include "CoreMinimal.h"
#include "FighterCapsule.generated.h"

USTRUCT(BlueprintType)
struct FFighterCapsule
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CollisionCapsule")
	float defaultHalfHeight = 90.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CollisionCapsule")
	float Radius = 30.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CollisionCapsule")
	float BottomOffset = 25.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CollisionCapsule")
	float TopOffset = 25.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CollisionCapsule")
	float bufferlayer = 0.05f;
	
	bool TopLowered = false;
	bool BottomLifted = false;
	
	FVector CenterPosition = FVector::ZeroVector;
	float HalfHeight;
	
	FVector GetCenter() const {return CenterPosition;}
	
	void UpdateCenter(const FVector& AnchorPoint)
	{
		CenterPosition = AnchorPoint + FVector(0.f, 0.f, defaultHalfHeight);
		if (TopLowered) CenterPosition.Z -= TopOffset;
		if (BottomLifted) CenterPosition.Z += BottomOffset;
	}
	
	void LowerTop()
	{
		if (TopLowered || BottomLifted) return;
		HalfHeight -= TopOffset;
		CenterPosition.Z += BottomOffset;
		TopLowered = true;
	}

	void LiftBottom()
	{
		if (TopLowered || BottomLifted) return;
		HalfHeight -= BottomOffset;
		CenterPosition.Z += BottomOffset;
		BottomLifted = true;
	}

	void Reset()
	{
		HalfHeight = defaultHalfHeight;
		TopLowered = false;
		BottomLifted = false;
	}

	FVector GetBottom() const
	{
		FVector Bottom = CenterPosition;
		Bottom.Z -= HalfHeight;
		return Bottom;
	}
};