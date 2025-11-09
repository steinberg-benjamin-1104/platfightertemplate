#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Procedural2DCapsuleComponent.h" // Or whatever your capsule component class is
#include "Hurtbox2DComponent.generated.h"

class AFighterPawn;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PFTEMPLATE_API UHurtbox2DComponent : public UProcedural2DCapsuleComponent
{
	GENERATED_BODY()

public:
	UHurtbox2DComponent(const FObjectInitializer& ObjectInitializer);

	/** Skeletal mesh to follow */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hurtbox")
	USkeletalMeshComponent* TargetSkeletalMesh = nullptr;

	/** Bone name to follow */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hurtbox")
	FName TargetBoneName;

	/** XZ offset relative to bone location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hurtbox")
	FVector2D XZOffset = FVector2D::ZeroVector;

	/** Rotation offset (degrees) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hurtbox")
	float RotationOffset = 0.f;

	/** Fighter reference for facing direction */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hurtbox")
	AFighterPawn* FighterPawnRef = nullptr;

	/** Is hurtbox active? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hurtbox")
	bool bHurtboxEnabled = true;

	/** Toggle hurtbox active/inactive */
	UFUNCTION(BlueprintCallable, Category = "Hurtbox")
	void ToggleHurtboxActive(bool bActive);

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void UpdateFromBone();
};