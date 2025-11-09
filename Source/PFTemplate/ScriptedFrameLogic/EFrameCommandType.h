#pragma once

#include "EFrameCommandType.generated.h"

UENUM(BlueprintType)
enum class EFrameCommandType : uint8
{
	None UMETA(DisplayName = "None"),
	SetCurrentAction UMETA(DisplayName = "SetCurrentAction"),
	TryChangeState UMETA(DisplayName = "TryChangeState"),
	SpawnHitboxes UMETA(DisplayName = "Spawn Hitbox"),
	ClearHitboxes UMETA(DisplayName = "Clear Hitboxes"),
	PlaySound UMETA(DisplayName = "Play Sound"),
	PlayVFX UMETA(DisplayName = "Trigger VFX"),
	AttackNotify UMETA(DisplayName = "AttackNotify"),
	SetVelocity UMETA(DisplayName = "Set Velocity"),
	StopVelocity UMETA(DisplayName = "Stop Velocity"),
	ToggleHurtboxI UMETA(DisplayName = "Toggle Hurtbox Invulnerability"),
	Custom UMETA(DisplayName = "Custom")
};