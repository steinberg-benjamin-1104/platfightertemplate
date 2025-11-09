#pragma once
#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
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

	void ApplyDamage(float Amount);
	
	bool IsBroken() const { return ShieldHealth <= 0.f; }
	void ResetHealth() {ShieldHealth = MaxShieldHealth;}
	float GetHealthPercent() const { return ShieldHealth / MaxShieldHealth; }

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Shield|Visual")
	float ShieldHeight = 80.f;

	UPROPERTY(EditDefaultsOnly, Category = "Shield|Visual")
	float ShieldScale = 1.7f;

	UPROPERTY()
	UMaterialInstanceDynamic* ShieldMaterial;
	
private:

	UPROPERTY() UStaticMeshComponent* ShieldMesh;
	
	UPROPERTY(EditAnywhere)
	float MaxShieldHealth = 60.f;

	UPROPERTY()
	float ShieldHealth;

	UPROPERTY(EditAnywhere)
	float RegenRate = 0.05f;

	UPROPERTY(EditAnywhere)
	float DegenerationRate = 0.3f;

	bool bIsActive = false;
	
	float BaseTranslucency = 0.7f;

	void UpdateMaterial();
};