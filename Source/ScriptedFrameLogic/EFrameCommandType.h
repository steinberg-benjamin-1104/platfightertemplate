#pragma once

#include "EFrameCommandType.generated.h"

UENUM(BlueprintType)
enum class EFrameCommandType : uint8
{
	SpawnHitboxes UMETA(DisplayName = "Spawn Hitbox"),
	ClearHitboxes UMETA(DisplayName = "Clear Hitboxes"),
	PlaySound UMETA(DisplayName = "Play Sound"),
	PlayVFX UMETA(DisplayName = "Trigger VFX"),
	AttackNotify UMETA(DisplayName = "AttackNotify"),
	SetVelocity UMETA(DisplayName = "Set Velocity"),
	StopVelocity UMETA(DisplayName = "Stop Velocity"),
	Custom UMETA(DisplayName = "Custom")
};