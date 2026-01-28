#pragma once

#include "CoreMinimal.h"
#include "HitboxData.h"
#include "InputBuffer.h"
#include "CharacterSnapshot.generated.h"

USTRUCT()
struct FCombatSnapshot
{
	GENERATED_BODY()

	int32 Health = 0;
	int32 Stocks = 3;
	int64 ShieldHealth;

	UPROPERTY() FDamageInfo StoredDamageInfo;
	UPROPERTY() AFighterPawn* LastInstigator;
};

USTRUCT()
struct FPhysicsSnapshot
{
	GENERATED_BODY()
	
	int64 LocationX;
	int64 LocationZ;
	int64 VelocityX;
	int64 VelocityZ;

	UPROPERTY() EFighterMovementMode CurrentMovementMode = EFighterMovementMode::Falling;
	bool bOnPlatform = false;
	bool bIgnorePlatform = false;
	bool bStopMovementUpdates = false;
	bool bCanLedge = true;
	bool bDoCollisionChecks = true;
	EJumpType CachedJumpType;
	int JumpsRemaining = 0;
	bool bIsFastFalling = false;
};

USTRUCT()
struct FFSMSnapshot
{
	GENERATED_BODY()

	int32 FramesInState;
	FName CurrentState;
};

USTRUCT()
struct FAnimSnapshot
{
	GENERATED_BODY()

	uint16 CurrentAnimID;
	uint16 PreviousAnimID;
	
	int32 CurrentFrame;
	int32 PreviousFrame;

	int32 CurrentNumFrames;
	int32 PreviousNumFrames;

	bool bCurrentLoop;
	bool bPreviousLoop;
	
	int32 BlendTotalFrames;
	int32 BlendFrameCounter;
	int64 BlendAlpha;
	
	bool bStopAnimUpdates;
};

struct FEffectSnapshot
{
	FName EffectID;
	int32 Duration;
	int32 RemainingFrames;
};

struct FEffectMachineSnapshot
{
	TArray<FEffectSnapshot> ActiveEffects;
};

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
