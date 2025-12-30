#pragma once

#include "CoreMinimal.h"
#include "Capsule2D.h"
#include "PendingHit.h"
#include "Hitbox2D.generated.h"

class AFighterPawn;
class USkeletalMeshComponent;

UCLASS()
class PFTEMPLATE_API AHitbox2D : public ACapsule2D
{
	GENERATED_BODY()

public:
	AHitbox2D();
	virtual void Initialize(APawn* InPawn) override;

	virtual void SetBoxActive(bool bActivate, const FHitboxDefinition&) override;
	void TickHitbox();
	const FHitboxDefinition& GetDefinition() const { return HitboxDefinition; }
	void GetHitPlayers(TArray<FPendingHit>& OutHits, TArray<TScriptInterface<IHittable>>& AlreadyHit) const;
	FDamageInfo GetDamageInfo() const { return HitboxDefinition.DamageInfo; }

protected:
	virtual void BeginPlay() override;
	
	virtual void UpdateLocation();
	virtual void UpdateRotation();

private:

	FHitboxDefinition HitboxDefinition;

	UPROPERTY(Blueprintable, EditAnywhere, Category = "Hitbox Materials")
	TMap<EHitboxType, FHitboxMaterialSet> MaterialSets;
	
	void ApplyMaterialForType();
	void UpdateTransform();

	UPROPERTY() AFighterPawn* FighterPawnRef = nullptr;
};