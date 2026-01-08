#include "FighterStateMachine.h"
#include "FighterPawn.h"
#include "FighterState.h"
#include "IdleState.h"
#include "BaseAttackState.h"
#include "JumpSquatState.h"
#include "DashState.h"
#include "RunState.h"
#include "WalkingState.h"
#include "SkidState.h"
#include "ChargeHeavyState.h"
#include "JumpUpState.h"
#include "FallingState.h"
#include "AirAttackState.h"
#include "AirdodgeState.h"
#include "HitstopState.h"
#include "JumpFromLedgeState.h"
#include "KnockbackState.h"
#include "KnockdownState.h"
#include "MiscAnimState.h"
#include "OffLedgeState.h"
#include "OnLedgeState.h"
#include "ParryState.h"
#include "PlatformDrop.h"
#include "ShieldbreakState.h"
#include "ShieldReleaseState.h"
#include "ShieldStartupState.h"
#include "ShieldState.h"
#include "SpecialState.h"
#include "TumbleState.h"

void UFighterStateMachine::Initialize(AFighterPawn* InOwner)
{
	StateMap.Add("Idle", NewObject<UIdleState>(this));
	StateMap.Add("BaseAttack", NewObject<UBaseAttackState>(this));
	StateMap.Add("JumpSquat", NewObject<UJumpSquatState>(this));
	StateMap.Add("Dash", NewObject<UDashState>(this));
	StateMap.Add("Run", NewObject<URunState>(this));
	StateMap.Add("Walking", NewObject<UWalkingState>(this));
	StateMap.Add("Skid", NewObject<USkidState>(this));
	StateMap.Add("ChargeHeavy", NewObject<UChargeHeavyState>(this));
	StateMap.Add("JumpUp", NewObject<UJumpUpState>(this));
	StateMap.Add("Falling", NewObject<UFallingState>(this));
	StateMap.Add("AirAttack", NewObject<UAirAttackState>(this));
	StateMap.Add("Shield", NewObject<UShieldState>(this));
	StateMap.Add("Knockback", NewObject<UKnockbackState>(this));
	StateMap.Add("Knockdown", NewObject<UKnockdownState>(this));
	StateMap.Add("OnLedge", NewObject<UOnLedgeState>(this));
	StateMap.Add("Hitstop", NewObject<UHitstopState>(this));
	StateMap.Add("Tumble", NewObject<UTumbleState>(this));
	StateMap.Add("Shieldbreak", NewObject<UShieldbreakState>(this));
	StateMap.Add("Parry", NewObject<UParryState>(this));
	StateMap.Add("Special", NewObject<USpecialState>(this));
	StateMap.Add("PlatformDrop", NewObject<UPlatformDropState>(this));
	StateMap.Add("Airdodge", NewObject<UAirDodgeState>(this));
	StateMap.Add("MiscAnim", NewObject<UMiscAnimState>(this));
	StateMap.Add("OffLedge", NewObject<UOffLedgeState>(this));
	StateMap.Add("JumpFromLedge", NewObject<UJumpFromLedgeState>(this));
	StateMap.Add("ShieldStartup", NewObject<UShieldStartupState>(this));
	StateMap.Add("ShieldRelease", NewObject<UShieldReleaseState>(this));
	
	InOwner->RegisterCustomStates(this);

	for (auto& Elem : StateMap)
	{
		if (Elem.Value)
			Elem.Value->InitState(InOwner, InOwner->MovementComponent, this, &InOwner->InputBuffer);
	}

	CurrentStateKey = "Idle";
	CurrentState = StateMap[CurrentStateKey];
	CurrentState->OnEnter();
}

void UFighterStateMachine::ChangeFighterState(FName NewState)
{
	if (!StateMap.Contains(NewState))
	{
		UE_LOG(LogTemp, Error, TEXT("State '%s' does not exist in StateMap!"), *NewState.ToString());
		return;
	}

	if (CurrentState->CanExitState() && StateMap[NewState]->CanEnterState())
	{
		CurrentState->OnExit();
		CurrentState = StateMap[NewState];
		CurrentStateKey = NewState;
		CurrentState->OnEnter();
		FramesInState = -1;
		TickCurrentState();
	}
}

void UFighterStateMachine::TickCurrentState()
{
	if (CurrentState)
	{
		FramesInState++;
		CurrentState->Tick(FramesInState);
	}
}

void UFighterStateMachine::ShowStateDebug()
{
	GEngine->AddOnScreenDebugMessage(
		-1,
		0.0f,
		FColor::Yellow,
		FString::Printf(TEXT("FSM State: %s"), *CurrentState->GetStateName())
	);
}

void UFighterStateMachine::AddState(FName StateName, UFighterState* State)
{
	if (!State || StateMap.Contains(StateName)) return;
	
	StateMap.Add(StateName, State);
}