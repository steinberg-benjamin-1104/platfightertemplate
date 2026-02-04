#include "CollisionWorld.h"
#include "CollisionMath.h"

void FDeterministicCollisionWorld::RegisterHitbox(const FHitbox InCapsule)
{
	HitboxBucket.Add(InCapsule);
}

void FDeterministicCollisionWorld::RegisterHurtbox(const FCollisionCapsule InCapsule)
{
	HurtboxBucket.Add(InCapsule);
}

void FDeterministicCollisionWorld::ResetHitboxBucket()
{
	HitboxBucket.Reset();
}

void FDeterministicCollisionWorld::ResetHurtboxBucket()
{
	HurtboxBucket.Reset();
}

FSweepResult FDeterministicCollisionWorld::ECBCollision(FPolygonCollision& CharacterECB, FFixedVector2D DesiredVelocity)
{
	for (auto Stage : StageGeometry)
	{
		FSweepResult CurrentHit = CollisionMath::SweepPolygonVsPolygon(CharacterECB, DesiredVelocity,*Stage);

		if (CurrentHit.bHit) return CurrentHit;
	}
}

void FDeterministicCollisionWorld::DetectHits()
{
	for (FHitbox Hitbox : HitboxBucket)
	{
		for (FCollisionCapsule Hurtbox : HurtboxBucket)
		{
			if (CollisionMath::CapsulesOverlap(Hitbox, Hurtbox))
			{
				PendingHits.Add({ &Hitbox, &Hurtbox });
			}
		}
	}
}