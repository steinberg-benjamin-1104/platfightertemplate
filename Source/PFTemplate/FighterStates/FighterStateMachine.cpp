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
#include "KnockbackState.h"
#include "KnockdownState.h"
#include "OnLedgeState.h"
#include "ParryState.h"
#include "PlatformDrop.h"
#include "ShieldbreakState.h"
#include "ShieldState.h"
#include "SpecialState.h"
#include "TumbleState.h"

void UFighterStateMachine::Initialize(AFighterPawn* InOwner)
{
	FighterPawnRef = InOwner;

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
	
	if (FighterPawnRef)
	{
		FighterPawnRef->RegisterCustomStates(this);
	}

	for (auto& Elem : StateMap)
	{
		if (Elem.Value)
			Elem.Value->InitState(FighterPawnRef, FighterPawnRef->MovementComponent, this);
	}

	CurrentStateKey = "Idle";
	CurrentState = StateMap[CurrentStateKey];
	FFighterInput NewInput;
	CurrentState->OnEnter(NewInput);
}

void UFighterStateMachine::ChangeFighterState(FName NewState, FFighterInput &TransitionInput)
{
	if (!StateMap.Contains(NewState) || !FighterPawnRef)
	{
		UE_LOG(LogTemp, Error, TEXT("State '%s' does not exist in StateMap!"), *NewState.ToString());
		return;
	}

	if (CurrentState->CanExitState() && StateMap[NewState]->CanEnterState())
	{
		CurrentState->OnExit();
		CurrentState = StateMap[NewState];
		CurrentStateKey = NewState;
		CurrentState->OnEnter(TransitionInput);
		FramesInState = -1;
		TickCurrentState(TransitionInput);
	}
}

void UFighterStateMachine::TickCurrentState(FFighterInput &Input)
{
	if (CurrentState && FighterPawnRef)
	{
		FramesInState++;
		CurrentState->Tick(Input, FramesInState);
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

	State->FighterPawnRef = FighterPawnRef;
	StateMap.Add(StateName, State);
}