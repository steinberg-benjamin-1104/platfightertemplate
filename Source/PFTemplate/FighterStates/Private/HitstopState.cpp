#include "HitstopState.h"

#include "FighterMovementComponent.h"
#include "FighterPawn.h"
#include "HitboxData.h"

void UHitstopState::OnEnter(FFighterInput& Input)
{
	FighterPawnRef->SetCurrentAnimation("Hitstop");
	FighterPawnRef->FaceInstigator();
	FighterPawnRef->UpdateAnimation(Input);
	FighterPawnRef->FreezePlayer(true);
	FighterPawnRef->LastInstigator->FreezePlayer(true);
	Duration = CalcHitstop();
	MoveComp->StopMovementCompletely();
}

bool UHitstopState::HandleTimer(FFighterInput& Input, int32 FramesInState)
{
	if (FramesInState == Duration)
	{
		StateMachine->ChangeFighterState("Knockback", Input);
		return true;
	}
	return false;
}

bool UHitstopState::HandlePhysics(FFighterInput& Input)
{
	FighterPawnRef->ShakeMesh();
	return false;
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
	return FixedFloor((DamageInfo.Damage/20 + 6) * DamageInfo.HitstopMod.ToFixed());
}