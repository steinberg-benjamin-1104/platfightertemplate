#include "FrameScriptRunner.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.h"
#include "HitboxManagerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

void UFrameScriptRunnerComponent::LoadScript(const FAnimation& NewAni, int inDuration)
{
	CurrentAction = NewAni;
	CommandIndex = 0;
	Duration = inDuration;
	CurrentFrame = 0;
	ResetAttackNotify();
	bIsFinished = false;
}

void UFrameScriptRunnerComponent::TickScript()
{
	++CurrentFrame;
	while (CommandIndex < CurrentAction.Commands.Num())
	{
		const FFrameCommand& Cmd = CurrentAction.Commands[CommandIndex];

		if (Cmd.FrameExecution > CurrentFrame)
			break;

		ExecuteCommand(Cmd);
		++CommandIndex;
	}

	if (Duration > 0 && CurrentFrame >= Duration)
	{
		if (CurrentAction.bIsLoop) CommandIndex = 0;
		else
		{
			if (CurrentAction.OnFinishCmd.Command != EFrameCommandType::None)
				ExecuteCommand(CurrentAction.OnFinishCmd);
			bIsFinished = true;
		}
	}
}

bool UFrameScriptRunnerComponent::IsFinished() const
{
	return bIsFinished;
}

void UFrameScriptRunnerComponent::ExecuteCommand(const FFrameCommand& Cmd)
{
	if (!FighterPawnRef) return;

	switch (Cmd.Command)
	{
		case EFrameCommandType::SetCurrentAction:
		{
			FighterPawnRef->SetCurrentAction(Cmd.NameParam, Cmd.IntParam);
		}

		case EFrameCommandType::TryChangeState:
		{
			FighterPawnRef->StateMachine->TryChangeState(Cmd.NameParam);
		}
		
		case EFrameCommandType::SpawnHitboxes:
		{
			if (CurrentAction.HitboxCollection.IsValidIndex(Cmd.IntParam))
			{
				const FHitboxGroup& HitboxGroup = CurrentAction.HitboxCollection[Cmd.IntParam];
				FighterPawnRef->GetHitboxManager()->ActivateHitboxes(HitboxGroup);
			}
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
			
		case EFrameCommandType::AttackNotify:
		{
			bAttackNotify = true;
			break;
		}

		case EFrameCommandType::SetVelocity:
		{
			const float Direction = FighterPawnRef->GetFacingDirection();
			FighterPawnRef->MovementComponent->SetVelocity(FVector(Cmd.VectorParam.X * Direction, 0.f, Cmd.VectorParam.Y));
			break;
		}

		case EFrameCommandType::StopVelocity:
		{
			FighterPawnRef->MovementComponent->SetVelocity(FVector::ZeroVector);
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