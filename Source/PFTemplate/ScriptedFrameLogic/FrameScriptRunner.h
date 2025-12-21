// FrameScriptRunner.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FrameCommand.h"
#include "FighterInput.h"
#include "FrameScriptRunner.generated.h"

class AFighterPawn;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PFTEMPLATE_API UFrameScriptRunnerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	void Initialize(AFighterPawn* pawn) {FighterPawnRef = pawn;}
	void LoadScript(const TArray<FFrameCommand>& NewCommands) {Commands = NewCommands;}
	void TickScript(FFighterInput& Input, int32 AnimFrame);

private:
	UPROPERTY() TArray<FFrameCommand> Commands;
	UPROPERTY() AFighterPawn* FighterPawnRef;
	int32 CommandIndex = 0;

	void ExecuteCommand(const FFrameCommand& Cmd, FFighterInput& Input);
};