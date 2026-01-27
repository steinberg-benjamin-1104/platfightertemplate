#pragma once
#include "CollisionDefinitions.h"

class FDeterministicCollisionWorld
{
	
public:
	void RegisterCapsule(const FCapsuleCollision& InCapsule);
	void RegisterPolygon(const FPolygonCollision& InPolygon);
    
	// Scans a specific component against the world
	void QueryOverlaps(const FCollisionComponent& QueryComp, TArray<uint32>& OutHitOwnerIDs) const;

	void QueryShapeOverlap(const FPolygonShape2D& TransientShape, ECollisionChannel TargetChannel, TArray<uint32>& OutHitOwnerIDs) const;

private:
	TArray<FCapsuleCollision> RegisteredCapsules;
	TArray<FPolygonCollision> RegisteredPolygons;
};