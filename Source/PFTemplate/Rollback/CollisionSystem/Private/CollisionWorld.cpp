#include "CollisionWorld.h"
#include "CollisionMath.h"

void FDeterministicCollisionWorld::RegisterHitbox(const FCapsuleCollision* InCapsule)
{
	HitboxBucket.Add(InCapsule);
}

void FDeterministicCollisionWorld::RegisterHurtbox(const FCapsuleCollision* InCapsule)
{
	HurtboxBucket.Add(InCapsule);
}

void FDeterministicCollisionWorld::QueryHitboxGroup(FHitboxGroupQuery& Group)
{
	for (const FCapsuleCollision& MyCapsule : Group.Capsules)
	{
		for (const FCapsuleCollision* TargetHurtbox : HurtboxBucket)
		{
			if (TargetHurtbox->OwnerID == Group.OwnerID) continue;
			if (Group.AlreadyHitOwners.Contains(TargetHurtbox->OwnerID)) continue;

			if (CollisionMath::CapsulesOverlap(MyCapsule, *TargetHurtbox))
			{
				Group.AlreadyHitOwners.Add(TargetHurtbox->OwnerID);
				break; 
			}
		}
		
		for (const FCapsuleCollision* OtherHitbox : HitboxBucket)
		{
			if (OtherHitbox->OwnerID == Group.OwnerID) continue;

			if (CollisionMath::CapsulesOverlap(MyCapsule, *OtherHitbox))
			{
				// Trigger Clash Logic (recoil, sparks, etc.)
			}
		}
	}
}

FSweepResult FDeterministicCollisionWorld::ECBCollision(FPolygonCollision& CharacterECB, FFixedVector2D DesiredVelocity)
{
	for (auto Stage : StageGeometry)
	{
		FSweepResult CurrentHit = CollisionMath::SweepPolygonVsPolygon(CharacterECB, DesiredVelocity,*Stage);

		if (CurrentHit.bHit) return CurrentHit;
	}
}