#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "Procedural2DCapsuleComponent.generated.h"

USTRUCT(BlueprintType)
struct FHitboxMaterialSet
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* FillMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* OutlineMaterial;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PFTEMPLATE_API UProcedural2DCapsuleComponent : public UProceduralMeshComponent
{
	GENERATED_BODY()

public:
	UProcedural2DCapsuleComponent(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void OnRegister() override;

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "2D Capsule Settings", meta=(DisplayName="Capsule Radius/HalfLength"))
	FVector2D CapsuleSize = {50.f, 100.f};

	UPROPERTY(Transient)
	int32 CircleSegments = 32;

	UPROPERTY(Transient)
	float OutlineThickness = 2.f;

	UFUNCTION(BlueprintCallable, Category = "2D Capsule Materials")
	void SetMaterialSet(const FHitboxMaterialSet& MaterialSet);
	
	UFUNCTION(BlueprintCallable, Category = "2D Capsule")
	void RebuildCapsule();


#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};