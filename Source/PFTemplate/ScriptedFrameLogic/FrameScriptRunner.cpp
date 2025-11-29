#include "FrameScriptRunner.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.h"
#include "HitboxManagerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

void UFrameScriptRunnerComponent::LoadScript(const TArray<FFrameCommand>& NewCommands, int inDuration, bool bNewLoop)
{
	Commands = NewCommands;
	CommandIndex = 0;
	Duration = inDuration;
	CurrentFrame = 0;
	bIsFinished = false;
	bLoop = bNewLoop;
}

void UFrameScriptRunnerComponent::TickScript(FFighterInput &Input)
{
	++CurrentFrame;
	while (CommandIndex < Commands.Num())
	{
		const FFrameCommand& Cmd = Commands[CommandIndex];

		if (Cmd.FrameExecution > CurrentFrame)
			break;

		ExecuteCommand(Cmd, Input);
		++CommandIndex;
	}

	if (Duration > 0 && CurrentFrame >= Duration)
	{
		if (bLoop) CommandIndex = 0;
		else bIsFinished = true;
	}
}

bool UFrameScriptRunnerComponent::IsFinished() const
{
	return bIsFinished;
}

void UFrameScriptRunnerComponent::ExecuteCommand(const FFrameCommand& Cmd, FFighterInput& Input)
{
	if (!FighterPawnRef) return;

	switch (Cmd.Command)
	{
		case EFrameCommandType::SetCurrentAnimation:
		{
			FighterPawnRef->SetCurrentAnimation(Cmd.NameParam, Cmd.IntParam);
			break;
		}

		case EFrameCommandType::TryChangeState:
		{
			FighterPawnRef->StateMachine->TryChangeState(Cmd.NameParam, Input);
			break;
		}
		
		case EFrameCommandType::SpawnHitboxes:
		{
			FighterPawnRef->GetHitboxManager()->ActivateHitboxes(Cmd.HitboxGroup);
			break;
		}

		case EFrameCommandType::ClearHitboxes:
		{
			FighterPawnRef->GetHitboxManager()->DeactivateHitboxes(Cmd.BoolParam); //clear hits?
			break;
		}

		case EFrameCommandType::PlaySound:
		{
			if (Cmd.Filepath.IsValid() || Cmd.Filepath.ToSoftObjectPath().IsValid())
            	{
            		UObject* Asset = Cmd.Filepath.LoadSynchronous();
            		if (USoundBase* Sound = Cast<USoundBase>(Asset))
            		{
            			UGameplayStatics::PlaySound2D(FighterPawnRef, Sound, 1.f);
            		}
            	}
			break;
		}

		case EFrameCommandType::PlayVFX:
		{
			if (Cmd.Filepath.IsValid() || Cmd.Filepath.ToSoftObjectPath().IsValid())
                {
                    UObject* Asset = Cmd.Filepath.LoadSynchronous();
                    if (UNiagaraSystem* Niagara = Cast<UNiagaraSystem>(Asset))
                    {
                     	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                     		FighterPawnRef->GetWorld(),
                     		Niagara,
                     		FighterPawnRef->GetActorLocation()
                     	);
                    }
                }
			break;
		}

		case EFrameCommandType::SetVelocity:
		{
			//figure out later
			break;
		}

		case EFrameCommandType::StopVelocity:
		{
			//figure out later
			break;
		}

		case EFrameCommandType::ToggleHurtboxI:
		{
			FighterPawnRef->ToggleHurtboxInvulnerable(Cmd.NameParam, Cmd.BoolParam);
		}
		
		default:
			break;
	}
}