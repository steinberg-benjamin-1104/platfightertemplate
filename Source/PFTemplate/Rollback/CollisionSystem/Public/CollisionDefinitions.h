#pragma once
#include "CoreMinimal.h"
#include "SafeMath.h"
#include "CollisionDefinitions.generated.h"

enum class EDeterministicCC : uint32
{
	None	= 0,
	ECB		= 1 << 0,
	Stage	= 1 << 1,
	Hitbox	= 1 << 2,
	Hurtbox	= 1 << 3,
	Ledge	= 1 << 4,
	Platform= 1 << 5
};

struct FCapsuleShape2D
{
	FFixedVector2D Axis;
	FFixed_32 HalfLength;
	FFixed_32 Radius;

	FCapsuleShape2D() : Axis(0, 0), HalfLength(20), Radius(5) {}

	void SetSize(FFixedVector2D InSize)
	{
		HalfLength = InSize.X;
		Radius = InSize.Z;
	}

	void SetRotation(FFixed_32 Degrees)
	{
		Axis.X = Degrees.Cos();
		Axis.Z = Degrees.Sin();
	}
};

struct FPolygonShape2D
{
	static const int MAX_VERTS = 8;
	FFixedVector2D Vertices[MAX_VERTS];
	int VertexCount = 0;
};

struct FCollisionComponent
{
	uint32 OwnerID;
	EDeterministicCC Channel;
	uint32 ResponseMask;
	FFixedVector2D WorldPos;

	FCollisionComponent(uint32 InOwnerID, EDeterministicCC InChannel, uint32 InMask)
	{
		OwnerID = InOwnerID;
		Channel = InChannel;
		ResponseMask = InMask;
		WorldPos = FFixedVector2D(0, 0);
	}
};

struct FCapsuleCollision : FCollisionComponent
{
	FCapsuleShape2D Capsule;
	bool bIsActive = true;
	bool bIgnoreOwner = true;

	FCapsuleCollision()
		: FCollisionComponent(0, EDeterministicCC::None, 0)
	{
		Capsule = FCapsuleShape2D();
		bIsActive = true;
		bIgnoreOwner = true;
	}
	
	FCapsuleCollision(uint32 InOwnerID, EDeterministicCC InChannel, uint32 InMask, FCapsuleShape2D InCapsuleShape, bool InActive)
		: FCollisionComponent(InOwnerID, InChannel, InMask)
	{
		Capsule = InCapsuleShape;
		bIsActive = InActive;
		bIgnoreOwner = true;
	}
};

struct FPolygonCollision : FCollisionComponent
{
	FPolygonShape2D Polygon;
};

struct FHitboxGroupQuery
{
	uint32 GroupID;
	uint32 OwnerID;
	TArray<FCapsuleCollision> Capsules;
	TArray<uint32> AlreadyHitOwners;
};

struct FSweepResult
{
	bool bHit = false;
	FFixedVector2D Normal = FFixedVector2D(0, 0);
	FFixedVector2D ImpactPoint = FFixedVector2D(0, 0);
	FFixed_32 Time;
};