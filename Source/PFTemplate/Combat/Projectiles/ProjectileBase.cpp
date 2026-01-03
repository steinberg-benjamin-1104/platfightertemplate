#include "ProjectileBase.h"
#include "FighterPawn.h"

AProjectileBase::AProjectileBase()
{
	PrimaryActorTick.bCanEverTick = false;

	Hitbox = CreateDefaultSubobject<UChildActorComponent>(TEXT("Hitbox"));
	Hitbox->SetupAttachment(RootComponent);
	Hitbox->SetChildActorClass(AHitbox2D::StaticClass());
	HitboxActor = Cast<AHitbox2D>(Hitbox->GetChildActor());
}

void AProjectileBase::Initialize(AFighterPawn* InPawn)
{
	FighterPawnRef = InPawn;
	HitboxActor->Initialize(InPawn);
	HitboxActor->SetBoxActive(false, FHitboxDefinition());
}

void AProjectileBase::ActivateProjectile(const FHitboxDefinition& InDefinition)
{
	Velocity = Vector2DToFixed2D(InitialVelocity) * FighterPawnRef->GetFacingDirection();
	LifeFrame = 0;
	HitboxActor->SetBoxActive(true, InDefinition);
}

void AProjectileBase::UpdateLocation()
{
	Velocity = Velocity + Vector2DToFixed2D(Acceleration) * FixedDt;
	
	FVector DesiredMove = Fixed2DToVector(Velocity * FixedDt);
	AddActorWorldOffset(DesiredMove, false);
}

void AProjectileBase::StepFrame()
{
	if (LifeFrame == MaxLifeFrame)
	{
		DeactivateProjectile();
		return;
	}

	UpdateLocation();
	
	HitboxActor->GetHitPlayers(PendingHits, HitObjects);
	LifeFrame++;
}

void AProjectileBase::DeactivateProjectile()
{
	Velocity = FFixedVector2D(0.f, 0.f);
	HitboxActor->SetBoxActive(false, FHitboxDefinition());
}

bool AProjectileBase::ProcessHits()
{
	if (PendingHits.IsEmpty()) return false;
	
	for (const FPendingHit& Hit : PendingHits)
	{
		Hit.HitObject->WasHit(Hit.DamageInfo, FighterPawnRef);
	}
	
	PendingHits.Empty();
	HitObjects.Reset();
	DeactivateProjectile();
	
	return true;
}