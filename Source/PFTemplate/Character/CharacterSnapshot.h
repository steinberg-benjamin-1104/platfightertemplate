#pragma once

#include "CoreMinimal.h"
#include "FighterPawn.h"
#include "InputBuffer.h"
#include "CharacterSnapshot.generated.h"

USTRUCT()
struct FCharacterSnapshot
{
	GENERATED_BODY()

	UPROPERTY() FCombatSnapshot Combat;
	UPROPERTY() FInputBuffer InputBuffer;
	UPROPERTY() FPhysicsSnapshot Physics;
	UPROPERTY() FFSMSnapshot FSM;
	UPROPERTY() FAnimSnapshot Anim;
	UPROPERTY() FEffectMachineSnapshot EM;
};