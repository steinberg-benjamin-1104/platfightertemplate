#include "FrameScriptRunner.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.h"
#include "HitboxManagerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

void UFrameScriptRunnerComponent::TickScript(int32 AnimFrame)
{
	if (AnimFrame == 1) CommandIndex = 0;
	while (CommandIndex < Commands.Num())
	{
		const FFrameCommand& Cmd = Commands[CommandIndex];

		if (Cmd.FrameExecution > AnimFrame)
			break;

		ExecuteCommand(Cmd);
		++CommandIndex;
	}
}

void UFrameScriptRunnerComponent::ExecuteCommand(const FFrameCommand& Cmd)
{
	if (!FighterPawnRef) return;

	switch (Cmd.Command)
	{
		case EFrameCommandType::SetCurrentAnimation:
		{
			FighterPawnRef->SetCurrentAnimation(Cmd.NameParam, Cmd.IntParam);
			break;
		}
		
		case EFrameCommandType::SpawnHitboxes:
		{
			FighterPawnRef->GetHitboxManager()->ActivateHitboxes(Cmd.NameParam);
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
                     		Fixed2DToVector(FighterPawnRef->GetFixedLoc())
                     	);
                    }
                }
			break;
		}

		case EFrameCommandType::SetDisplacement:
		{
			FFixedVector2D Displacement = Cmd.VectorParam.ToFixed();
			Displacement.X *= FighterPawnRef->GetFacingDirection();
			FighterPawnRef->MovementComponent->ManualDisplacement(Displacement, Cmd.BoolParam);
		}

		case EFrameCommandType::ToggleHurtboxI:
		{
			FighterPawnRef->ToggleHurtboxInvulnerable(Cmd.NameParam, Cmd.BoolParam);
		}
		
		default:
			break;
	}
}