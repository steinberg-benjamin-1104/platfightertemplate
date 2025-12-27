#include "FighterState.h"
#include "FighterPawn.h"

void UFighterState::InitState(AFighterPawn* InFighterPawn, UFighterMovementComponent* InMoveComp, UFighterStateMachine* InStateMachine, FInputBuffer* InInputBuffer)
{
	FighterPawnRef = InFighterPawn;
	MoveComp = InMoveComp;
	StateMachine = InStateMachine;
	InputBuffer = InInputBuffer;
}

EStickDir UFighterState::GetCurrentStickDir()
{
	return GetStickDirection(InputBuffer->GetRecent().StickPos, FighterPawnRef->IsFacingRight());
}

bool UFighterState::CheckActionButtons()
{
	EInputButton AttackButtons[] = { EInputButton::Attack, EInputButton::Special };
	for (EInputButton Btn : AttackButtons)
	{
		if (FFighterInput* Input = InputBuffer->WasPressed(Btn))
		{
			if (FighterPawnRef->TryStartAttack(Btn, Input))
			{
				Input->Consume(Btn);
				return true;
			}
		}
	}
	return false;
}


bool UFighterState::CheckBufferedButtonStateChanges(const TMap<EInputButton, FName>& TransitionMap)
{
	if (!InputBuffer || !StateMachine) return false;

	for (const auto& Pair : TransitionMap)
	{
		if (FFighterInput* Input = InputBuffer->WasPressed(Pair.Key))
		{
			Input->Consume(Pair.Key);
			StateMachine->ChangeFighterState(Pair.Value);
			return true;
		}
	}
	return false;
}