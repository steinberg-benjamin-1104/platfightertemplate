#pragma once

#include "CoreMinimal.h"
#include "Procedural2DCapsuleComponent.h"
#include "GameFramework/Actor.h"
#include "HitboxData.h"

#include "Capsule2D.generated.h"

class UProcedural2DCapsuleComponent;
class UCapsuleComponent;

UCLASS(Blueprintable)
class PFTEMPLATE_API ACapsule2D : public AActor
{
    GENERATED_BODY()

public:
    ACapsule2D();

    virtual void Initialize(APawn* InPawn) { SetOwner(InPawn); }

protected:
    virtual void OnConstruction(const FTransform& Transform) override;
#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "2D Capsule")
    FVector2D CapsuleSize = FVector2D(50.f, 100.f);

    UFUNCTION(BlueprintCallable, Category = "2D Capsule")
    FVector2D GetCapsuleSize() const { return CapsuleMesh->CapsuleSize; }
    
    UFUNCTION(BlueprintCallable, Category = "2D Capsule")
    virtual void SetCapsuleSize(FVector2D NewCapsuleSize);

    UFUNCTION(BlueprintCallable, Category = "2D Capsule")
    void SetDebugVisible(bool bVisible);

protected:
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "2D Capsule Components")
    UProcedural2DCapsuleComponent* CapsuleMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "2D Capsule Components")
    UCapsuleComponent* CollisionCapsule;
};