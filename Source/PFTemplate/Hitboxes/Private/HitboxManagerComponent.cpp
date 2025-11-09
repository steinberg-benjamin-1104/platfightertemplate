#include "HitboxManagerComponent.h"
#include "Hitbox2D.h"
#include "FighterPawn.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

UHitboxManagerComponent::UHitboxManagerComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UHitboxManagerComponent::Initialize(AFighterPawn* InOwner, int32 PoolSize)
{
    FighterPawnRef = InOwner;

    if (HitboxClass && GetWorld())
    {
        for (int32 i = 0; i < PoolSize; ++i)
        {
            AHitbox2D* NewHitbox = GetWorld()->SpawnActor<AHitbox2D>(HitboxClass);
            if (NewHitbox)
            {
                NewHitbox->Initialize(InOwner);
                NewHitbox->SetBoxActive(false, FHitboxDefinition());
                HitboxPool.Add(NewHitbox);
            }
        }
    }
}

void UHitboxManagerComponent::ScanActiveHitboxes()
{
    for (AHitbox2D* Hitbox : ActiveHitboxGroup)
    {
        Hitbox->TickHitbox();
        Hitbox->GetHitPlayers(PendingHits, HitObjects);
    }
}

void UHitboxManagerComponent::ProcessHits()
{
    for (const FPendingHit& Hit : PendingHits)
    {
        Hit.HitObject->WasHit(Hit.DamageInfo, FighterPawnRef);
    }
    PendingHits.Empty();
}

AHitbox2D* UHitboxManagerComponent::RequestHitbox()
{
    for (AHitbox2D* Hitbox : HitboxPool)
    {
        if (Hitbox && !Hitbox->IsActive())
        {
            return Hitbox;
        }
    }

    UE_LOG(LogTemp, Fatal, TEXT("Hitbox pool exhausted! Increase pool size in Initialize()."));
    return nullptr;
}

void UHitboxManagerComponent::ActivateHitboxes(const FHitboxGroup& HitboxGroup)
{
    for (const FHitboxDefinition& HitboxDef : HitboxGroup.Hitboxes)
    {
        AHitbox2D* Hitbox = RequestHitbox();
        if (!Hitbox) continue;

        Hitbox->SetBoxActive(true, HitboxDef);
        ActiveHitboxGroup.Add(Hitbox);
    }
}

void UHitboxManagerComponent::DeactivateHitboxes(bool ClearHits)
{
    for (AHitbox2D* Hitbox : ActiveHitboxGroup)
    {
        if (Hitbox)
        {
            Hitbox->SetBoxActive(false, FHitboxDefinition());
        }
    }
    ActiveHitboxGroup.Reset();
    if (ClearHits) ResetAlraedyHitObjects();
}