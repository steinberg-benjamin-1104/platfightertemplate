#pragma once
#include "CoreMinimal.h"
#include "Fixed32.h"
#include "FixedVector2D.h"
#include "SafeMathBP.generated.h"

USTRUCT(BlueprintType)
struct FFixed_32BP
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float Value = 0.f;

	FFixed_32BP() = default; 
	FFixed_32BP(float InValue)
		: Value(InValue)
	{}

	FFixed_32 ToFixed() const { return FFixed_32(Value); }

	operator FFixed_32() const { return FFixed_32(Value); }
    
};

USTRUCT(BlueprintType)
struct FVec2DBP
{
	GENERATED_BODY()

	// These are what the data table will expose
	UPROPERTY(EditAnywhere)
	float X = 0.f;

	UPROPERTY(EditAnywhere)
	float Z = 0.f;

	// Convert to your fixed vector
	FFixedVector2D ToFixed() const
	{
		return FFixedVector2D(X, Z);
	}
};

FORCEINLINE FFixed_32 operator*(const FFixed_32& a, const FFixed_32BP& b)
{
	return a * FFixed_32(b);
}

FORCEINLINE FFixed_32 operator*(const FFixed_32BP& a, const FFixed_32& b)
{
	return FFixed_32(a) * b;
}

inline FFixed_32 operator*(const FFixed_32BP& a, const FFixed_32BP& b)
{
	return FFixed_32(a) * FFixed_32(b);
}