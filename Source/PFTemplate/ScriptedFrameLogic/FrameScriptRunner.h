// FrameScriptRunner.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FrameCommand.h"
#include "Animation.h"
#include "FighterInput.h"
#include "FrameScriptRunner.generated.h"

class AFighterPawn;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PFTEMPLATE_API UFrameScriptRunnerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	void Initialize(AFighterPawn* pawn) {FighterPawnRef = pawn;}
	void LoadScript(const TArray<FFrameCommand>& Commands, int inDuration, bool bNewLoop);
	void TickScript(FFighterInput& Input);
	bool IsFinished() const;

private:
	UPROPERTY() TArray<FFrameCommand> Commands;
	UPROPERTY() AFighterPawn* FighterPawnRef;
	int32 Duration = 0;
	int32 CurrentFrame = 0;
	
	int32 CommandIndex = 0;
	
	bool bIsFinished = false;
	bool bLoop = false;

	void ExecuteCommand(const FFrameCommand& Cmd, FFighterInput& Input);
};