// FixedCollision.h
#pragma once
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
						Fixed2DToVector(Start),
						Fixed2DToVector(End),
						Channel);

	FFixedHitResult Out;
	Out.bBlockingHit = bHit;
	if (bHit)
	{
		Out.Position  = VectorToFixed2D(Hit.Location);
		Out.Normal    = VectorToFixed2D(Hit.Normal);
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
						Fixed2DToVector(Start),
						Fixed2DToVector(End),
						FQuat::Identity,
						Channel,
						FCollisionShape::MakeBox(FVector(FixedToFloat(HalfExtent.X),
														 FixedToFloat(HalfExtent.Z),
														 0.f)));

	FFixedHitResult Out;
	Out.bBlockingHit = bHit;
	if (bHit)
	{
		Out.Position  = VectorToFixed2D(Hit.Location);
		Out.Normal    = VectorToFixed2D(Hit.Normal);
		Out.Distance  = FloatToFixed(Hit.Distance);
	}
	return Out;
}

static FFixedHitResult FixedSweepCapsule(const UWorld* World,
										 const FFixedVector2D& Start,
										 const FFixedVector2D& End,
										 FIXED_32 Radius,
										 FIXED_32 HalfHeight,
										 ECollisionChannel Channel = ECC_Pawn)
{
	FHitResult Hit;
	const bool bHit = World->SweepSingleByChannel(
						Hit,
						Fixed2DToVector(Start),
						Fixed2DToVector(End),
						FQuat::Identity,
						Channel,
						FCollisionShape::MakeCapsule(FixedToFloat(Radius),
													 FixedToFloat(HalfHeight)));

	FFixedHitResult Out;
	Out.bBlockingHit = bHit;
	if (bHit)
	{
		Out.Position  = VectorToFixed2D(Hit.Location);
		Out.Normal    = VectorToFixed2D(Hit.Normal);
		Out.Distance  = FloatToFixed(Hit.Distance);
	}
	return Out;
}