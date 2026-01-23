#pragma once
#include "CoreMinimal.h"
#include "SafeMath.h"
#include "CollisionDefinitions.generated.h"

// --- Core Types ---
enum class EDeterministicCC : uint32
{
	None      = 0,
	ECB       = 1 << 0, // Environmental Collision Box (The Player's "Solid" box)
	Stage     = 1 << 1, // Static Stage Geometry
	Hitbox    = 1 << 2, // Offensive shapes
	Hurtbox   = 1 << 3  // Defensive shapes
};

struct FCapsuleShape2D
{
	FFixedVector2D Axis; // Normalized direction of the spine
	FFixed_32 HalfLength;
	FFixed_32 Radius;
};

struct FPolygonShape2D {
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
    
	// We use a simple toggle; in a production build, use a Union
	bool IsCapsule; 
	FCapsuleShape2D Capsule;
	FPolygonShape2D Polygon;
};