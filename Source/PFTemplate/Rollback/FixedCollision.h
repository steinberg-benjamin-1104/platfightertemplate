// FixedCollision.h
#pragma once
#include "Engine/World.h"

struct FFixedHitResult
{
	FFixedVector2D Position;
	FFixedVector2D Normal;
	FFixed_32        Distance;
	bool            bBlockingHit = false;
};

constexpr ECollisionChannel Channel = ECC_WorldStatic;

static FFixedHitResult FixedLineTrace(const UWorld* World,
									  const FFixedVector2D& Start,
									  const FFixedVector2D& End)
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
									 const FFixedVector2D& HalfExtent)
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
										 FFixed_32 Radius,
										 FFixed_32 HalfHeight)
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

template<typename T>
static T* FixedOverlapBoxFirstActorOfClass(const UWorld* World,
										   const FFixedVector2D& Center,
										   const FFixedVector2D& HalfExtent)
{
	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(FixedOverlapBoxFirstActorOfClass), false);

	World->OverlapMultiByChannel(
		Overlaps,
		Fixed2DToVector(Center),
		FQuat::Identity,
		Channel,
		FCollisionShape::MakeBox(FVector(
			FixedToFloat(HalfExtent.X),
			FixedToFloat(HalfExtent.Z),
			0.f)),
		Params
	);

	for (const FOverlapResult& O : Overlaps)
	{
		if (AActor* A = O.GetActor())
		{
			if (T* Casted = Cast<T>(A))
			{
				return Casted;
			}
		}
	}

	return nullptr;
}