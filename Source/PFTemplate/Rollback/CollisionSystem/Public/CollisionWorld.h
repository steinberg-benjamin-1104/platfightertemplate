#pragma once
#include "CollisionDefinitions.h"

struct FPendingHit
{
	const FHitbox* Hitbox;
	const FCollisionCapsule* Hurtbox;
};

class FDeterministicCollisionWorld
{
	
public:
	void ResetHitboxBucket();
	void ResetHurtboxBucket();
	void RegisterHitbox(const FHitbox InCapsule);
	void RegisterHurtbox(const FCollisionCapsule InCapsule);
	void DetectHits();

	FSweepResult ECBCollision(FPolygonCollision& CharacterECB, FFixedVector2D DesiredVelocity);

	TArray<FPendingHit> PendingHits;


private:
	TArray<const FCollisionCapsule> HurtboxBucket; 
	TArray<const FHitbox> HitboxBucket;
	TArray<const FPolygonCollision*> StageGeometry;
};