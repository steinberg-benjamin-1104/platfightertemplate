#pragma once

#include "CoreMinimal.h"
#include "FighterState.h"
#include "ShieldbreakState.generated.h"

UCLASS()
class PFTEMPLATE_API UShieldbreakState : public UFighterState
{
    GENERATED_BODY()
    
public:
    void OnEnter() override;
    bool HandleTimer(int32 FramesInState) override;
    void OnExit() override;

    FString GetStateName() override { return "Shieldbreak";}

private:
    int Duration = 120;
};