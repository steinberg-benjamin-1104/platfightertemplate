#pragma once

#include "CoreMinimal.h"
#include "Capsule2D.h"
#include "BoneVectorAxis.h"
#include "Hurtbox2D.generated.h"

class USkeletalMeshComponent;
class AFighterPawn;

UCLASS(Blueprintable)
class PFTEMPLATE_API AHurtbox2D : public ACapsule2D
{
    GENERATED_BODY()

public:
    AHurtbox2D();
    void TickHurtbox();

    virtual void Initialize(APawn* InOwner, FDeterministicCollisionWorld* CW) override;
    
    UFUNCTION(BlueprintCallable, Category = "Hurtbox")
    void UpdateLocation();
    
    UFUNCTION(BlueprintCallable, Category = "Hurtbox")
    void UpdateRotation();
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hurtbox")
    FName BoneName = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hurtbox")
    FHitboxMaterialSet ActiveColors;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hurtbox")
    FHitboxMaterialSet InvulnerableColors;

    void SetInvincibility(bool bInvulnerable);
    bool IsInvulnerable() const {return bIsInvincible;}
    
private:
    bool bIsInvincible = false;

    UPROPERTY() AFighterPawn* FighterPawnRef = nullptr;
};