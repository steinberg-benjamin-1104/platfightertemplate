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
	ECollisionChannel Channel;
	uint32 ResponseMask;
	FFixedVector2D WorldPosition;
};

struct FCapsuleCollision : FCollisionComponent
{
	FCapsuleShape2D Capsule;
	bool bIsActive = true;
	bool bIgnoreOwner = true;
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