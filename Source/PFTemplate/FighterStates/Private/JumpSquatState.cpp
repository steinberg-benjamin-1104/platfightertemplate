#include "JumpSquatState.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.h"

void UJumpSquatState::OnEnter()
{
	bJumpButtonHeld = true;
	FighterPawnRef->SetCurrentAction("JumpSquat");
}

bool UJumpSquatState::JumpReleased()
{
	if (!bJumpButtonHeld) return false;
	bJumpButtonHeld = false;
	return true;
}

void UJumpSquatState::Tick()
{
	Super::Tick();

	if (StateMachine->FramesInState == Duration)
	{
		StateMachine->TryChangeState("Rising");
		return;
	}
	
	MoveComp->ApplyGroundFriction();
	MoveComp->HandleLedgeOrFall(true);
	
}

void UJumpSquatState::OnExit()
{
	MoveComp->DoHop(bJumpButtonHeld ? EHopType::Full : EHopType::Short);
}