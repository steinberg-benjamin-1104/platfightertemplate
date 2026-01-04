#include "HitstopState.h"

#include "FighterMovementComponent.h"
#include "FighterPawn.h"
#include "HitboxData.h"

void UHitstopState::OnEnter()
{
	FighterPawnRef->SetCurrentAnimation("Hitstop");
	FighterPawnRef->FaceInstigator();
	FighterPawnRef->UpdateAnimation();
	FighterPawnRef->FreezePlayer(true);
	Duration = CalcHitstop();
	MoveComp->StopMovementCompletely();
}

bool UHitstopState::HandleTimer(int32 FramesInState)
{
	if (FramesInState == Duration)
	{
		StateMachine->ChangeFighterState("Knockback");
		return true;
	}
	return false;
}

bool UHitstopState::HandlePhysics()
{
	FighterPawnRef->ShakeMesh();
	return false;
}

void UHitstopState::OnExit()
{
	FighterPawnRef->ResetMeshOffset();
	FighterPawnRef->FreezePlayer(false);
	FighterPawnRef->LastInstigator->FreezePlayer(false);
}

int UHitstopState::CalcHitstop()
{
	FDamageInfo DamageInfo = FighterPawnRef->StoredDamageInfo;
	FFixed_32 HitstopMod = FFixed_32(DamageInfo.HitstopMod)/FFixed_32(100.f);
	return FixedFloor((DamageInfo.Damage/20 + 6) * HitstopMod);
}