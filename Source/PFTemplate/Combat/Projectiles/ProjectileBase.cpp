#include "ProjectileBase.h"
#include "FighterPawn.h"

AProjectileBase::AProjectileBase()
{
	PrimaryActorTick.bCanEverTick = false;

	Hitbox = CreateDefaultSubobject<UChildActorComponent>(TEXT("Hitbox"));
	Hitbox->SetupAttachment(RootComponent);
	Hitbox->SetChildActorClass(AHitbox2D::StaticClass());
	HitboxActor = Cast<AHitbox2D>(Hitbox->GetChildActor());
	
	//Mesh
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetGenerateOverlapEvents(false);
}

void AProjectileBase::Initialize(AFighterPawn* InPawn)
{
	FighterPawnRef = InPawn;
	HitboxActor->Initialize(InPawn);
	HitboxActor->SetBoxActive(false, FHitboxDefinition());
	Mesh->SetVisibility(false);
}

void AProjectileBase::ActivateProjectile(const FHitboxDefinition& InDefinition)
{
	Velocity = Vector2DToFixed2D(InitialVelocity) * FighterPawnRef->GetFacingDirection();
	LifeFrame = 0;
	HitboxActor->SetBoxActive(true, InDefinition);

	if (Mesh)
	{
		Mesh->SetRelativeRotation(FRotator(0.f, (Velocity.X >= 0) ? 0.f : 180.f, 0.f));
		Mesh->SetVisibility(true);
	}
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
	Mesh->SetVisibility(false);
}

bool AProjectileBase::ProcessHits()
{
	if (PendingHits.IsEmpty())
	{
		LifeFrame++;
		return false;
	}
	
	for (const FPendingHit& Hit : PendingHits)
	{
		AFighterPawn* fighter = Cast<AFighterPawn>(Hit.HitObject.GetObject());
		fighter->SetFacingDirection(Velocity.X.Sign());
		Hit.HitObject->WasHit(Hit.DamageInfo, FighterPawnRef);
	}
	
	PendingHits.Empty();
	HitObjects.Reset();
	DeactivateProjectile();
	
	return true;
}

bool AProjectileBase::PreCollision()
{
	if (LifeFrame == MaxLifeFrame)
	{
		DeactivateProjectile();
		return true;
	}
	
	UpdateLocation();
	return false;
}

void AProjectileBase::DetectCollisions()
{
	HitboxActor->GetHitPlayers(PendingHits, HitObjects);
}