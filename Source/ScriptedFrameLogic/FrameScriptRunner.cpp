#include "FrameScriptRunner.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.h"
#include "HitboxManagerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

void UFrameScriptRunnerComponent::LoadScript(const TArray<FFrameCommand>& InScript, const TArray<FHitboxGroup>& HitboxDefs, bool looping, int inDuration)
{
	bIsLooping = looping;
	Script = InScript;
	Hitboxes = HitboxDefs;
	CurrentFrame = 1;
	CommandIndex = 0;
	Duration = inDuration;
	ResetAttackNotify();
}

void UFrameScriptRunnerComponent::TickScript()
{
	// Process any commands scheduled for *this* frame
	while (CommandIndex < Script.Num())
	{
		const FFrameCommand& Cmd = Script[CommandIndex];

		if (Cmd.FrameExecution > CurrentFrame)
			break;

		ExecuteCommand(Cmd);
		++CommandIndex;
	}

	if (Duration > 0 && CurrentFrame >= Duration)
	{
		if (bIsLooping)
		{
			CurrentFrame = 0;
			CommandIndex = 0;
		}
	}

	++CurrentFrame;
}

bool UFrameScriptRunnerComponent::IsFinished() const
{
	return Duration > 0 && CurrentFrame >= Duration;
}

void UFrameScriptRunnerComponent::ExecuteCommand(const FFrameCommand& Cmd)
{
	if (!FighterPawnRef) return;

	switch (Cmd.Command)
	{
		case EFrameCommandType::SpawnHitboxes:
		{
			if (Hitboxes.IsValidIndex(Cmd.IntParam))
			{
				const FHitboxGroup& HitboxGroup = Hitboxes[Cmd.IntParam];
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
			const float Direction = FighterPawnRef->IsFacingRight() ? 1.f : -1.f;
			FighterPawnRef->MovementComponent->SetVelocity(FVector(Cmd.VectorParam.X * Direction, 0.f, Cmd.VectorParam.Y));
			break;
		}

		case EFrameCommandType::StopVelocity:
		{
			FighterPawnRef->MovementComponent->SetVelocity(FVector::ZeroVector);
			break;
		}
		
		default:
			break;
	}
}