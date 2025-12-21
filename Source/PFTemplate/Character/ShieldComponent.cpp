#include "ShieldComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/StaticMeshComponent.h"

UShieldComponent::UShieldComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	ShieldHealth = MaxShieldHealth;
}

void UShieldComponent::InitShield(UStaticMeshComponent* InMesh)
{
	ShieldMesh = InMesh;
	if (ShieldMesh && ShieldMesh->GetMaterial(0))
	{
		ShieldMaterial = UMaterialInstanceDynamic::Create(ShieldMesh->GetMaterial(0), this);
		ShieldMesh->SetMaterial(0, ShieldMaterial);
	}
}

void UShieldComponent::SetShieldActive(bool bActive)
{
	bIsActive = bActive;
	ShieldMesh->SetVisibility(bActive);
	if (bActive) UpdateMaterial();
}

void UShieldComponent::ApplyDamage(int32 Amount)
{
	ShieldHealth = FixedMax(FFixed_32(0), ShieldHealth - FFixed_32(Amount));
	UpdateMaterial();
}

void UShieldComponent::UpdateShield()
{
	if (bIsActive)
	{
		ShieldHealth = FixedMax(FFixed_32(0.f), ShieldHealth - DegenerationRate);
		UpdateMaterial();
	}
	else
	{
		ShieldHealth = FixedMin(MaxShieldHealth, ShieldHealth + RegenRate);
	}
}

void UShieldComponent::UpdateMaterial()
{
	if (!ShieldMaterial) return;
	ShieldMaterial->SetScalarParameterValue(TEXT("Alpha"), FixedToFloat(ShieldHealth / MaxShieldHealth));
}