#pragma once

#include "CoreMinimal.h"
#include "FighterState.h"
#include "ShieldbreakState.generated.h"

UCLASS()
class PFTEMPLATE_API UShieldbreakState : public UFighterState
{
    GENERATED_BODY()
    
public:
    void OnEnter(FFighterInput& Input) override;
    bool HandleTimer(FFighterInput& Input, int32 FramesInState) override;
    void OnExit() override;

private:
    int Duration = 120;
};