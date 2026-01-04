#pragma once

#include "EFrameCommandType.generated.h"

UENUM(BlueprintType)
enum class EFrameCommandType : uint8
{
	None UMETA(DisplayName = "None"),
	SetCurrentAnimation UMETA(DisplayName = "SetCurrentAnimation"),
	ChangeFighterState UMETA(DisplayName = "ChangeFighterState"),
	SpawnHitboxes UMETA(DisplayName = "Spawn Hitbox"),
	ClearHitboxes UMETA(DisplayName = "Clear Hitboxes"),
	PlaySound UMETA(DisplayName = "Play Sound"),
	PlayVFX UMETA(DisplayName = "Trigger VFX"),
	SetDisplacement UMETA(DisplayName = "Set Displacement"),
	ToggleHurtboxI UMETA(DisplayName = "Toggle Hurtbox Invulnerability"),
	SpawnProjectile UMETA(DisplayName = "Spawn Projectile"),
	Custom UMETA(DisplayName = "Custom") //doing nothing with this right now
};