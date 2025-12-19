#include "RollbackSimSubsystem.h"
#include "BattleManager.h"

void URollbackSimSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	FixedStepTime   = 0.016666667f;
	TimeAccumulator = 0.0f;

	WorldTickHandle =
		FWorldDelegates::OnWorldTickStart.AddUObject(
			this,
			&URollbackSimSubsystem::OnWorldTick
		);
}

void URollbackSimSubsystem::Deinitialize()
{
	if (WorldTickHandle.IsValid())
	{
		FWorldDelegates::OnWorldTickStart.Remove(WorldTickHandle);
		WorldTickHandle.Reset();
	}

	Super::Deinitialize();
}

void URollbackSimSubsystem::OnWorldTick(
	UWorld* World,
	ELevelTick TickType,
	float DeltaSeconds)
{
	// Only tick the actual game world
	if (!World || !World->IsGameWorld()) return;

	TimeAccumulator += DeltaSeconds;

	constexpr int32 MaxStepsPerTick = 8;
	int32 Steps = 0;

	while (TimeAccumulator >= FixedStepTime && Steps < MaxStepsPerTick)
	{
		FixedTick();
		TimeAccumulator -= FixedStepTime;
		Steps++;
	}
}

void URollbackSimSubsystem::FixedTick()
{
	if (BattleManager)
	{
		BattleManager->StepFrame();
	}
}

void URollbackSimSubsystem::RegisterBattleManager(ABattleManager* InManager)
{
	BattleManager = InManager;
}