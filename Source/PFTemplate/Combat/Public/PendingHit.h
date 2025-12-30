#pragma once

#include "CoreMinimal.h"
#include "Hittable.h"
#include "PendingHit.generated.h"

USTRUCT(BlueprintType)
struct FPendingHit
{
	GENERATED_BODY();

	UPROPERTY()
	TScriptInterface<IHittable> HitObject;

	UPROPERTY()
	FDamageInfo DamageInfo;

	FPendingHit() = default;

	FPendingHit(const TScriptInterface<IHittable>& InHitObject, const FDamageInfo& InDamageInfo)
		: HitObject(InHitObject)
		, DamageInfo(InDamageInfo)
	{
	}
};
