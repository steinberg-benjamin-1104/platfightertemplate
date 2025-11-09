// FrameScriptRunner.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FrameCommand.h"
#include "HitboxData.h"
#include "FrameScriptRunner.generated.h"

class AFighterPawn;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UFrameScriptRunnerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	void Initialize(AFighterPawn* pawn) {FighterPawnRef = pawn;}
	void LoadScript(const TArray<FFrameCommand>& InScript, const TArray<FHitboxGroup>& HitboxDefs, bool looping, int inDuration);
	void TickScript();
	bool IsFinished() const;
	bool AttackNotify() const {return bAttackNotify;}
	void ResetAttackNotify() {bAttackNotify = false;}

private:
	bool bIsLooping = false;
	TArray<FFrameCommand> Script;
	TArray<FHitboxGroup> Hitboxes;
	UPROPERTY() AFighterPawn* FighterPawnRef;
	int32 Duration = 0;

	int32 CurrentFrame = 0;
	int32 CommandIndex = 0;

	bool bAttackNotify = false;

	void ExecuteCommand(const FFrameCommand& Cmd);
};