#pragma once

#include "CoreMinimal.h"
#include "MiscAnimState.h"
#include "OffLedgeState.generated.h"

UCLASS()
class PFTEMPLATE_API UOffLedgeState : public UMiscAnimState
{
    GENERATED_BODY()
    
public:
    virtual void OnEnter() override;
    virtual void OnExit() override;
};