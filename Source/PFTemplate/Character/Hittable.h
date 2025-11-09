#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HitboxData.h"
#include "Hittable.generated.h"


class AFighterPawn;
UINTERFACE(MinimalAPI, Blueprintable)
class UHittable : public UInterface
{
	GENERATED_BODY()
};

class PFTEMPLATE_API IHittable
{
	GENERATED_BODY()

public:
	virtual void WasHit(const FDamageInfo& DamageInfo, AFighterPawn* Instigator) = 0;
};