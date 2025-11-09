#pragma once

#include "CoreMinimal.h"
#include "Input.generated.h"

UENUM(BlueprintType)
enum class EInputButtonType : uint8
{
	None, Attack, Special, Jump, Shield, Grab
};

UENUM(BlueprintType)
enum class EStickDirection : uint8
{
	Neutral  UMETA(DisplayName = "Neutral"),
	Left     UMETA(DisplayName = "Left"),
	Right    UMETA(DisplayName = "Right"),
	Up       UMETA(DisplayName = "Up"),
	Down     UMETA(DisplayName = "Down")
};

USTRUCT(BlueprintType)
struct FStickInt8
{
	GENERATED_BODY()
	
	int8 X;
	int8 Y;

	// Default constructor
	FStickInt8() : X(0), Y(0) {}
	FStickInt8(int8 InX, int8 InY) : X(InX), Y(InY) {}

	// Equality operators
	bool operator==(const FStickInt8& Other) const
	{
		return X == Other.X && Y == Other.Y;
	}

	bool operator!=(const FStickInt8& Other) const
	{
		return !(*this == Other);
	}

	// Conversion to float vector for rendering / movement
	FVector2D ToFloat() const
	{
		return FVector2D(static_cast<float>(X) / 127.f,
						 static_cast<float>(Y) / 127.f);
	}

	// Static constant zero value
	static const FStickInt8 ZeroVector;
};

// Define the static const
inline const FStickInt8 FStickInt8::ZeroVector(0, 0);


USTRUCT(BlueprintType)
struct FBufferedInput
{
	GENERATED_BODY()
	
	UPROPERTY()
	EInputButtonType Button;

	UPROPERTY()
	FStickInt8 StickInput;

	UPROPERTY()
	int32 FrameNumber;
};