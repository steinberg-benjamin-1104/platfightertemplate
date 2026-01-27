#pragma once
#include "CollisionDefinitions.h"

class FDeterministicCollisionWorld
{
	
public:
	void RegisterHitbox(const FCapsuleCollision InCapsule);
	void RegisterHurtbox(const FCapsuleCollision InCapsule);
	
	void RegisterStageGeometry(const FPolygonCollision InPolygon);
	
	void QueryHitboxGroup(FHitboxGroupQuery& Group);

	FSweepResult ECBCollision(FPolygonCollision& CharacterECB, FFixedVector2D DesiredVelocity);

private:
	TArray<FCapsuleCollision> HurtboxBucket; 
	TArray<FCapsuleCollision> HitboxBucket;
	TArray<FPolygonCollision> StageGeometry;
};