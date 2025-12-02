#pragma once
#include "CoreMinimal.h"
#include "SafeMathBP.h"
#include "FighterCapsule.generated.h"

USTRUCT(BlueprintType)
struct FFighterCapsule
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CollisionCapsule")
	FFixed_32BP defaultHalfHeight = 90.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CollisionCapsule")
	FFixed_32BP Radius = 30.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CollisionCapsule")
	FFixed_32BP BottomOffset = 25.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CollisionCapsule")
	FFixed_32BP TopOffset = 25.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CollisionCapsule")
	FFixed_32BP bufferlayer = 0.05f;
	
	bool TopLowered = false;
	bool BottomLifted = false;
	
	FFixedVector2D CenterPosition;
	FFixed_32 HalfHeight;
	
	FFixedVector2D GetCenter() const {return CenterPosition;}
	
	void UpdateCenter(const FFixedVector2D& AnchorPoint)
	{
		CenterPosition = AnchorPoint + FFixedVector2D(0.f, defaultHalfHeight);
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

	FFixedVector2D GetBottom() const
	{
		FFixedVector2D Bottom = CenterPosition;
		Bottom.Z -= HalfHeight;
		return Bottom;
	}
};