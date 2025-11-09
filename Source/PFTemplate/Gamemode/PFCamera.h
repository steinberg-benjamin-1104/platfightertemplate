#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "BoxBounds.h"
#include "PFCamera.generated.h"


class ICamTarget;

UCLASS()
class PFTEMPLATE_API APFCamera : public AActor
{
    GENERATED_BODY()

public:
    APFCamera();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* RootMesh;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USpringArmComponent* SpringArm;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UCameraComponent* CameraComp;
    
    UFUNCTION(BlueprintCallable, Category = "Camera Targets")
    void AddTargetActor(AActor* Actor);

    UFUNCTION(BlueprintCallable, Category = "Camera Targets")
    void RemoveTargetActor(AActor* Actor);

    void UpdateCamera();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float PanSpeed = 10.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float ZoomSpeed = 2.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float MinArmLength = 800.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float MaxArmLength = 2400.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    FBoxBounds CameraBounds;

private:
    UPROPERTY() TArray<TScriptInterface<ICamTarget>> CamTargets;
    
    void UpdateCameraLocation(const FBoxBounds& TargetBounds);
    void UpdateCameraZoom(const FBoxBounds& TargetBounds);
    void DrawDebugBounds(const FBoxBounds& Bounds) const;
    FBoxBounds CalcCameraProjectionBounds(FVector Location) const;
    FBoxBounds CalcTargetBounds() const;
    void CalcHalfProjection(float Dist, float& OutHalfW, float& OutHalfH) const;
    FVector ClampLocationToBounds(const FVector& Loc) const;

    // State
    FVector PrevCenter = FVector::ZeroVector;
    FVector PrevVel    = FVector::ZeroVector;

    float BaseYaw = -90.0f;
    float BasePitch = 0.0f;

    float CeilingPadding = 350.f;
    float Padding = 200.f;

    static constexpr float FixedFrameTime = 1.0f / 60.0f;
};