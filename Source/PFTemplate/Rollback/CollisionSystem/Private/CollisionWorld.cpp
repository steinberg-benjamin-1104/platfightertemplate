#include "CollisionWorld.h"
#include "CollisionMath.h"

void FDeterministicCollisionWorld::Reset() {
	RegisteredComponents.Reset();
}

void FDeterministicCollisionWorld::RegisterComponent(const FCollisionComponent& InComponent) {
	RegisteredComponents.Add(InComponent);
}

void FDeterministicCollisionWorld::QueryOverlaps(const FCollisionComponent& QueryComp, TArray<uint32>& OutHitOwnerIDs) const {
	for (const auto& Target : RegisteredComponents) {
		// 1. Filter logic
		if (QueryComp.OwnerID == Target.OwnerID) continue;
		if (!(QueryComp.ResponseMask & (uint32)Target.Channel)) continue;

		bool bOverlapped = false;

		// 2. Purely routing based on type
		if (QueryComp.IsCapsule && Target.IsCapsule) {
			bOverlapped = CollisionMath::CapsulesOverlap(QueryComp, Target);
		}
		else if (!QueryComp.IsCapsule && !Target.IsCapsule) {
			bOverlapped = CollisionMath::PolygonsOverlap(QueryComp, Target);
		}

		if (bOverlapped) {
			OutHitOwnerIDs.Add(Target.OwnerID);
		}
	}
}