#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "RollbackSimSubsystem.generated.h"

class ABattleManager;

UCLASS()
class PFTEMPLATE_API URollbackSimSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    void RegisterBattleManager(class ABattleManager* InManager);

private:
    void OnWorldTick(UWorld* World, ELevelTick TickType, float DeltaSeconds);
    void FixedTick();

private:
    UPROPERTY()
    ABattleManager* BattleManager = nullptr;

    FDelegateHandle WorldTickHandle;

    float FixedStepTime = 0.f;
    float TimeAccumulator = 0.f;
};