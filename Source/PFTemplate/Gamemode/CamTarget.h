#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CamTarget.generated.h"

//wrapper class cause UE is dumb
UINTERFACE(MinimalAPI, Blueprintable)
class UCamTarget : public UInterface
{
	GENERATED_BODY()
};

class PFTEMPLATE_API ICamTarget
{
	GENERATED_BODY()

public:
	virtual FVector CenterLocation() = 0;
};