#pragma once
#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "SafeMath.h"
#include "ShieldComponent.generated.h"

UCLASS()
class PFTEMPLATE_API UShieldComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UShieldComponent();
	void InitShield(UStaticMeshComponent* InMesh);
	
	void UpdateShield();
	
	void SetShieldActive(bool bActive);
	bool IsActive() const { return bIsActive; }

	void ApplyDamage(int32 Amount);
	
	bool IsBroken() const { return ShieldHealth <= 0.f; }
	void ResetHealth() {ShieldHealth = MaxShieldHealth;}
	FIXED_32 GetHealthPercent() const { return ShieldHealth / MaxShieldHealth; }

protected:
	
	UPROPERTY()
	UMaterialInstanceDynamic* ShieldMaterial;
	
private:

	UPROPERTY() UStaticMeshComponent* ShieldMesh;
	
	FIXED_32 MaxShieldHealth = 600.f;
	FIXED_32 ShieldHealth;
	FIXED_32 RegenRate = 0.05f;
	FIXED_32 DegenerationRate = 0.3f;

	bool bIsActive = false;

	void UpdateMaterial();
};