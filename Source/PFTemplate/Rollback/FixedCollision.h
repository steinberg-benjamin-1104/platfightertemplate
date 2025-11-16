// FixedCollision.h
#pragma once
#include "SafeMath.h"
#include "Engine/World.h"

struct FFixedHitResult
{
	FFixedVector2D Position;
	FFixedVector2D Normal;
	FIXED_32        Distance;
	bool            bBlockingHit = false;
};

static FFixedHitResult FixedLineTrace(const UWorld* World,
									  const FFixedVector2D& Start,
									  const FFixedVector2D& End,
									  const ECollisionChannel Channel = ECC_Visibility)
{
	FHitResult Hit;
	const bool bHit = World->LineTraceSingleByChannel(
						Hit,
						ToFloat(Start),
						ToFloat(End),
						Channel);

	FFixedHitResult Out;
	Out.bBlockingHit = bHit;
	if (bHit)
	{
		Out.Position  = ToFixed(Hit.Location);
		Out.Normal    = ToFixed(Hit.Normal);
		Out.Distance  = FloatToFixed(Hit.Distance);
	}
	return Out;
}

static FFixedHitResult FixedSweepBox(const UWorld* World,
									 const FFixedVector2D& Start,
									 const FFixedVector2D& End,
									 const FFixedVector2D& HalfExtent,
									 const ECollisionChannel Channel = ECC_Pawn)
{
	FHitResult Hit;
	const bool bHit = World->SweepSingleByChannel(
						Hit,
						ToFloat(Start),
						ToFloat(End),
						FQuat::Identity,
						Channel,
						FCollisionShape::MakeBox(FVector(FixedToFloat(HalfExtent.X),
														 FixedToFloat(HalfExtent.Z),
														 0.f)));

	FFixedHitResult Out;
	Out.bBlockingHit = bHit;
	if (bHit)
	{
		Out.Position = ToFixed(Hit.Location);
		Out.Normal   = ToFixed(Hit.Normal);
		Out.Distance = FloatToFixed(Hit.Distance);
	}
	return Out;
}

static FFixedHitResult FixedSweepCapsule(const UWorld* World,
										 const FFixedVector2D& Start,
										 const FFixedVector2D& End,
										 FIXED_32 Radius,
										 FIXED_32 HalfHeight,   // vertical half-height
										 ECollisionChannel Channel = ECC_Pawn)
{
	FHitResult Hit;
	const bool bHit = World->SweepSingleByChannel(
						Hit,
						ToFloat(Start),
						ToFloat(End),
						FQuat::Identity,               // upright capsule
						Channel,
						FCollisionShape::MakeCapsule(FixedToFloat(Radius),
													 FixedToFloat(HalfHeight)));

	FFixedHitResult Out;
	Out.bBlockingHit = bHit;
	if (bHit)
	{
		Out.Position = ToFixed(Hit.Location);
		Out.Normal   = ToFixed(Hit.Normal);
		Out.Distance = FloatToFixed(Hit.Distance);
	}
	return Out;
}