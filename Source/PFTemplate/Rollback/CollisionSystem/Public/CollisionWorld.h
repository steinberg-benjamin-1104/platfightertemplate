#pragma once
#include "CollisionDefinitions.h"

class FDeterministicCollisionWorld {
public:
	void Reset();
	void RegisterComponent(const FCollisionComponent& InComponent);
    
	// Scans a specific component against the world
	void QueryOverlaps(const FCollisionComponent& QueryComp, TArray<uint32>& OutHitOwnerIDs) const;

private:
	// Fixed size array is more deterministic/performant for rollback than TMap
	TArray<FCollisionComponent> RegisteredComponents;
};