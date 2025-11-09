#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HitboxData.h"
#include "Hittable.h"
#include "PendingHit.h"
#include "HitboxManagerComponent.generated.h"

class AFighterPawn;
class AHitbox2D;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PFTEMPLATE_API UHitboxManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHitboxManagerComponent();

	void Initialize(AFighterPawn* InOwner, int32 PoolSize = 16);

	void ActivateHitboxes(const FHitboxGroup& HitboxGroup);
	void ScanActiveHitboxes();
	void ProcessHits();
	void DeactivateHitboxes(bool ClearHits);
	void ResetAlraedyHitObjects() {HitObjects.Reset();}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hitbox")
	TSubclassOf<AHitbox2D> HitboxClass;

private:
	UPROPERTY()
	TArray<AHitbox2D*> HitboxPool;

	UPROPERTY()
	TArray<AHitbox2D*> ActiveHitboxGroup;
	
	TArray<TScriptInterface<IHittable>> HitObjects;

	UPROPERTY()
	TArray<FPendingHit> PendingHits;

	AFighterPawn* FighterPawnRef = nullptr;

	AHitbox2D* RequestHitbox();
};