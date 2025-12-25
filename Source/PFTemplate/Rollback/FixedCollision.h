// FixedCollision.h
#pragma once
#include "CoreMinimal.h"
#include "Engine/World.h"
#include "Engine/EngineTypes.h"
#include "Engine/OverlapResult.h"

struct FFixedHitResult
{
	FFixedVector2D ImpactPoint;
	FFixedVector2D Normal;
	FFixed_32        Distance;
	bool            bBlockingHit = false;
	AActor* HitActor = nullptr;
};

constexpr ECollisionChannel Channel = ECC_WorldStatic;

static FFixedHitResult FixedLineTrace(
	const UWorld* World,
	const FFixedVector2D& Start,
	const FFixedVector2D& End,
	bool bDebug = false)
{
	FHitResult Hit;

	const FVector VStart = Fixed2DToVector(Start);
	const FVector VEnd   = Fixed2DToVector(End);

	const bool bHit = World->LineTraceSingleByChannel(Hit, VStart, VEnd, Channel);

	// Debug visualization
	if (bDebug)
	{
		const FColor LineColor = bHit ? FColor::Green : FColor::Red;
		DrawDebugLine(World, VStart,
			VEnd,
			LineColor,
			false,
			1.5f,
			0,
			1.5f
		);

		// Hit point
		if (bHit)
		{
			DrawDebugPoint(
				World,
				Hit.Location,
				10.f,
				FColor::Yellow,
				false,
				1.5f
			);
		}
	}

	FFixedHitResult Out;
	Out.bBlockingHit = bHit;

	if (bHit)
	{
		Out.ImpactPoint = VectorToFixed2D(Hit.Location);
		Out.Normal   = VectorToFixed2D(Hit.Normal);
		Out.Distance = FloatToFixed(Hit.Distance);
		Out.HitActor = Hit.GetActor();
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
		Out.ImpactPoint  = VectorToFixed2D(Hit.Location);
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
		Out.ImpactPoint  = VectorToFixed2D(Hit.ImpactPoint);
		Out.Normal    = VectorToFixed2D(Hit.Normal);
		Out.Distance  = FloatToFixed(Hit.Distance);
		Out.HitActor = Hit.GetActor();
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
		ECC_WorldDynamic, //temp collision channel
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