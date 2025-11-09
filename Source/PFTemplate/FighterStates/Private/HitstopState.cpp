#include "HitstopState.h"

#include "FighterMovementComponent.h"
#include "FighterPawn.h"
#include "HitboxData.h"

void UHitstopState::OnEnter()
{
	FighterPawnRef->SetCurrentAction("Hitstop");
	FighterPawnRef->FaceInstigator();
	FighterPawnRef->UpdateAnimation();
	FighterPawnRef->FreezePlayer(true);
	FighterPawnRef->LastInstigator->FreezePlayer(true);
	Duration = CalcHitstop();
	MoveComp->StopMovementCompletely();
}

void UHitstopState::Tick()
{
	if (StateMachine->FramesInState == Duration)
	{
		StateMachine->TryChangeState("Knockback");
		return;
	}
	
	FighterPawnRef->ShakeMesh();
}

void UHitstopState::OnExit()
{
	FighterPawnRef->ResetMeshOffset();
	FighterPawnRef->FreezePlayer(false);
	FighterPawnRef->LastInstigator->FreezePlayer(false);
	Duration = CalcHitstop();
}

int UHitstopState::CalcHitstop()
{
	FDamageInfo DamageInfo = FighterPawnRef->StoredDamageInfo;
	return FMath::FloorToInt((DamageInfo.Damage/20 + 6) * DamageInfo.HitstopMod);
}