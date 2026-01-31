#pragma once
#include "CoreMinimal.h"
#include "SafeMath.h"
#include "CollisionDefinitions.generated.h"

//ECB Collision Channels
enum class EECBCC : uint32
{
	None	= 0,
	Stage	= 1 << 0,
	Platform= 1 << 1,
	Ledge	= 1 << 2,
	ECB		= 1 << 3
};

struct FCapsuleShape2D
{
	FFixedVector2D Axis;
	FFixed_32 HalfLength;
	FFixed_32 Radius;

	FCapsuleShape2D() : Axis(0, 0), HalfLength(20), Radius(5) {}

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
	FFixedVector2D WorldPos;

	FCollisionComponent(uint32 InOwnerID, FFixedVector2D InWorldPos)
	{
		OwnerID = InOwnerID;
		WorldPos = InWorldPos;
	}
};

struct FCollisionCapsule : FCollisionComponent
{
	FCapsuleShape2D Capsule;

	FCollisionCapsule(uint32 InOwner, FFixedVector2D InWorldPos, const FCapsuleShape2D& InCapsule) : FCollisionComponent(InOwner, InWorldPos)
	{
		Capsule = InCapsule;
	}

	void SetRotation(FFixed_32 Degrees) { Capsule.SetRotation(Degrees); }
};

struct FHitbox : FCollisionCapsule
{
	uint32 InstigatorID;
	uint16 GroupID;
	uint16 Index;
	
	FHitbox(uint32 InOwner, const FCapsuleShape2D& InCapsule, FFixedVector2D InWorldPos, uint32 InInstigator, uint16 InGroupID, uint16 InIndex)
	: FCollisionCapsule(InOwner, InWorldPos, InCapsule)
	{
		InstigatorID = InInstigator;
		GroupID = InGroupID;
		Index = InIndex;
	}
};

struct FPolygonCollision : FCollisionComponent
{
	FPolygonShape2D Polygon;
	EECBCC ChannelID;
	uint32 ResponseMask;
};

struct FSweepResult
{
	bool bHit = false;
	FFixedVector2D Normal = FFixedVector2D(0, 0);
	FFixedVector2D ImpactPoint = FFixedVector2D(0, 0);
	FFixed_32 Time;
};