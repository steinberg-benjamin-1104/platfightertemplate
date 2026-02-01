#pragma once
#include "CollisionDefinitions.h"

class FDeterministicCollisionWorld
{
	
public:
	void ResetHitboxBucket();
	void ResetHurtboxBucket();
	void RegisterHitbox(const FHitbox InCapsule);
	void RegisterHurtbox(const FCollisionCapsule InCapsule);

	FSweepResult ECBCollision(FPolygonCollision& CharacterECB, FFixedVector2D DesiredVelocity);

private:
	TArray<const FCollisionCapsule> HurtboxBucket; 
	TArray<const FHitbox> HitboxBucket;
	TArray<const FPolygonCollision*> StageGeometry;
};