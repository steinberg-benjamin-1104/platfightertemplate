#include "CollisionWorld.h"
#include "CollisionMath.h"

void FDeterministicCollisionWorld::RegisterCapsule(const FCapsuleCollision& InCapsule)
{
	RegisteredCapsules.Add(InCapsule);
}

void FDeterministicCollisionWorld::RegisterPolygon(const FPolygonCollision& InPolygon)
{
	RegisteredPolygons.Add(InPolygon);
}

void FDeterministicCollisionWorld::QueryOverlaps(const FCollisionComponent& QueryComp, TArray<uint32>& OutHitOwnerIDs) const {
	for (const auto& Target : RegisteredComponents)
	{
		// 1. Filter logic
		if (QueryComp.OwnerID == Target.OwnerID) continue;
		if (!(QueryComp.ResponseMask & (uint32)Target.Channel)) continue;

		bool bOverlapped = false;

		// 2. Purely routing based on type
		if (QueryComp.IsCapsule && Target.IsCapsule) {
			bOverlapped = CollisionMath::CapsulesOverlap(QueryComp, Target);
		}
		else if (!QueryComp.IsCapsule && !Target.IsCapsule)
		{
			bOverlapped = CollisionMath::PolygonsOverlap(QueryComp, Target);
		}

		if (bOverlapped)
		{
			OutHitOwnerIDs.Add(Target.OwnerID);
		}
	}
}

// Add this to your FDeterministicCollisionWorld
void FDeterministicCollisionWorld::QueryShapeOverlap(const FPolygonShape2D& TransientShape, ECollisionChannel TargetChannel, TArray<uint32>& OutHitOwnerIDs) const {
	// We don't need a full Component, just the geometry and a mask
	for (const auto& Target : RegisteredPolygons) {
		// Only check against the Ledge channel
		if (!(Target.Channel & (uint32)TargetChannel)) continue;

		if (CollisionMath::PolygonsOverlap(TransientShape, Target.Polygon)) {
			OutHitOwnerIDs.Add(Target.OwnerID);
		}
	}
}