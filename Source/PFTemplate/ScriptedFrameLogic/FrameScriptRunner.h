// FrameScriptRunner.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FrameCommand.h"
#include "Animation.h"
#include "FrameScriptRunner.generated.h"

class AFighterPawn;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PFTEMPLATE_API UFrameScriptRunnerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	void Initialize(AFighterPawn* pawn) {FighterPawnRef = pawn;}
	void LoadScript(const FAction& NewAction, int inDuration);
	void TickScript();
	bool IsFinished() const;
	bool AttackNotify() const {return bAttackNotify;}
	void ResetAttackNotify() {bAttackNotify = false;}

private:
	FAction CurrentAction;
	UPROPERTY() AFighterPawn* FighterPawnRef;
	int32 Duration = 0;
	int32 CurrentFrame = 0;
	
	int32 CommandIndex = 0;

	bool bAttackNotify = false;
	bool bIsFinished = false;

	void ExecuteCommand(const FFrameCommand& Cmd);
};