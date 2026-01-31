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

	FCollisionComponent(uint32 InOwnerID)
	{
		OwnerID = InOwnerID;
		WorldPos = FFixedVector2D(0, 0);
	}

	FFixedVector2D GetWorldPos() const { return WorldPos; }
	void SetWorldPos(FFixedVector2D NewPos) { WorldPos = NewPos; }
};

struct FHitbox : FCollisionComponent
{
	FCapsuleShape2D Capsule;
	FFixedVector2D OffsetFromBone;
	bool bIsAttached = true;
	FName BoneName;
	int32 Lifespan;
	
	FHitbox(uint32 InOwnerID, const FCapsuleShape2D& InCapsuleShape, bool InAttach, FName Bone, int32 InLife) : FCollisionComponent(InOwnerID)
	{
		Capsule = InCapsuleShape;
		bIsAttached = InAttach;
		BoneName = Bone;
		Lifespan = InLife;
	}

	void SetRotation(FFixed_32 Degrees) { Capsule.SetRotation(Degrees); }
};

struct FHurtbox : FCollisionComponent
{
	FCapsuleShape2D Capsule;
	bool bIsActive = true;
	FName BoneName;

	FHurtbox(uint32 InOwnerID, const FCapsuleShape2D& InCapsuleShape, FName Bone) : FCollisionComponent(InOwnerID)
	{
		Capsule = InCapsuleShape;
		BoneName = Bone;
	}
	
	void SetRotation(FFixed_32 Degrees) { Capsule.SetRotation(Degrees); }
};

struct FPolygonCollision : FCollisionComponent
{
	FPolygonShape2D Polygon;
	EECBCC ChannelID;
	uint32 ResponseMask;
};

struct FHitboxGroupQuery
{
	uint32 GroupID;
	uint32 OwnerID;
	TArray<FHitbox> Capsules;
	TArray<uint32> AlreadyHitOwners;
};

struct FSweepResult
{
	bool bHit = false;
	FFixedVector2D Normal = FFixedVector2D(0, 0);
	FFixedVector2D ImpactPoint = FFixedVector2D(0, 0);
	FFixed_32 Time;
};