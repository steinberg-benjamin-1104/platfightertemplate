#pragma once

#include "CoreMinimal.h"
#include "Procedural2DCapsuleComponent.h"
#include "GameFramework/Actor.h"
#include "SafeMath.h"
#include "CollisionDefinitions.h"
#include "CollisionWorld.h"

#include "Capsule2D.generated.h"

class UProcedural2DCapsuleComponent;

UCLASS(Blueprintable)
class PFTEMPLATE_API ACapsule2D : public AActor
{
    GENERATED_BODY()

public:
    ACapsule2D();

    virtual void Initialize(APawn* InPawn, FDeterministicCollisionWorld* CW)
    {
        SetOwner(InPawn);
        CollisionWorld = CW;
    }

protected:
    virtual void OnConstruction(const FTransform& Transform) override;
#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "2D Capsule")
    FFixedVector2D CapsuleSize = FFixedVector2D(50.f, 100.f);

    UFUNCTION(BlueprintCallable, Category = "2D Capsule")
    FFixedVector2D GetCapsuleSize() const { return CapsuleSize; }
    
    UFUNCTION(BlueprintCallable, Category = "2D Capsule")
    virtual void SetCapsuleSize(FFixedVector2D NewCapsuleSize);

    void SetCapsuleRotation(FFixed_32 Rotation);

    void SetCapsuleLocation(FFixedVector2D NewLoc);

    UFUNCTION(BlueprintCallable, Category = "2D Capsule")
    void SetDebugVisible(bool bVisible);

protected:
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "2D Capsule Components")
    UProcedural2DCapsuleComponent* CapsuleMesh;

    FCapsuleCollision CollisionCapsule;

private:
    FDeterministicCollisionWorld* CollisionWorld;
};