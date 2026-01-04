#include "ProjectileBase.h"
#include "FighterPawn.h"
#include "SafeMath.h"

AProjectileBase::AProjectileBase()
{
	PrimaryActorTick.bCanEverTick = false;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetGenerateOverlapEvents(false);
}

void AProjectileBase::Initialize(AFighterPawn* InPawn)
{
	FighterPawnRef = InPawn;
	
	HitboxActor = GetWorld()->SpawnActor<AHitbox2D>(FighterPawnRef->GetHitboxManager()->HitboxClass);
	HitboxActor->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
	HitboxActor->Initialize(InPawn);
	HitboxActor->SetBoxActive(false, FHitboxDefinition());
	
	Mesh->SetVisibility(false);
}

void AProjectileBase::ActivateProjectile(const FHitboxDefinition& InDefinition)
{
	SetSpawnLoc();
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

void AProjectileBase::SetSpawnLoc()
{
	FFixedVector2D Offset = Vector2DToFixed2D(SpawnLocOffset);

	LogFixedVector2D("InitialOffset", Offset);

	Offset.X = Offset.X * FighterPawnRef->GetFacingDirection();
	FFixedVector2D FinalLoc = FighterPawnRef->GetFixedLoc() + Offset;
	LogFixedVector2D("Spawn Loc", FinalLoc);
	SetActorLocation(Fixed2DToVector(FinalLoc));
}