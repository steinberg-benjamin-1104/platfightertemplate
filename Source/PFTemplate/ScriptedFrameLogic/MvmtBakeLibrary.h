#pragma once

#include "CoreMinimal.h"
#include "AnimMvmt.h"
#include "MvmtBakeLibrary.generated.h"

UCLASS()
class UMvmtBakeLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Fighter|Baking")
    static void BakeMovementData(UBakedAnimMvmt* DataAsset)
    {
        if (!DataAsset || !DataAsset->X.Curve || !DataAsset->Z.Curve)
        {
            UE_LOG(LogTemp, Warning, TEXT("BakeMovementData: Missing DataAsset or Curves."));
            return;
        }

        DataAsset->DeltaPos.Empty();
        
        const int32 TotalFrames = DataAsset->EndFrame - DataAsset->StartFrame;
        
        if (TotalFrames <= 0) return;

        const float MagX = DataAsset->X.TotalDisplacement;
        const float MagZ = DataAsset->Z.TotalDisplacement;

        // Keep track of the position at the previous frame to calculate the change
        FVector2D LastFramePos = FVector2D::ZeroVector;

        // Start from i = 1 because the delta at frame 0 is always 0
        // We initialize the array with a zero vector for the first frame
        DataAsset->DeltaPos.Add(FVector2D::ZeroVector);

        for (int32 i = 1; i <= TotalFrames; ++i)
        {
            const float Alpha = static_cast<float>(i) / static_cast<float>(TotalFrames);

            // 1. Calculate the Absolute Position for the current frame
            float CurrentX = DataAsset->X.Curve->GetFloatValue(Alpha) * MagX;
            float CurrentZ = DataAsset->Z.Curve->GetFloatValue(Alpha) * MagZ;
            FVector2D CurrentFramePos = FVector2D(CurrentX, CurrentZ);

            // 2. Calculate the Delta (Current - Previous)
            FVector2D Delta = CurrentFramePos - LastFramePos;

            // 3. Store the Delta and update the tracker
            DataAsset->DeltaPos.Add(Delta);
            LastFramePos = CurrentFramePos;
        }

#if WITH_EDITOR
        DataAsset->MarkPackageDirty();
#endif

        UE_LOG(LogTemp, Log, TEXT("BakeMovementData: Successfully baked %d deltas into %s"), 
            DataAsset->DeltaPos.Num(), *DataAsset->GetName());
    }
};