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

	void ActivateHitboxes(FName GroupName);
	void ScanActiveHitboxes();
	void ProcessHits();
	void DeactivateHitboxes(bool ClearHits);
	void ResetAlraedyHitObjects() {HitObjects.Reset();}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AHitbox2D> HitboxClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UDataTable* HitboxTable;

private:
	UPROPERTY()
	TArray<AHitbox2D*> HitboxPool;

	UPROPERTY()
	TArray<AHitbox2D*> ActiveHitboxGroup;
	
	TArray<TScriptInterface<IHittable>> HitObjects;

	UPROPERTY()
	TArray<FPendingHit> PendingHits;

	AHitbox2D* RequestHitbox();

	UPROPERTY()
	AFighterPawn* FighterPawnRef = nullptr;
};