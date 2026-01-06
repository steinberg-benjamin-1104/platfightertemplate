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
        if (!DataAsset) return;

        const bool bHasX = DataAsset->X.Curve != nullptr;
        const bool bHasZ = DataAsset->Z.Curve != nullptr;

        if (!bHasX && !bHasZ) return;

        // 1. Calculate and store the Bake bounds
        if (bHasX && bHasZ)
        {
            DataAsset->BakeStartFrame = FMath::Min(DataAsset->X.StartFrame, DataAsset->Z.StartFrame);
            DataAsset->BakeEndFrame = FMath::Max(DataAsset->X.EndFrame, DataAsset->Z.EndFrame);
        }
        else
        {
            DataAsset->BakeStartFrame = bHasX ? DataAsset->X.StartFrame : DataAsset->Z.StartFrame;
            DataAsset->BakeEndFrame = bHasX ? DataAsset->X.EndFrame : DataAsset->Z.EndFrame;
        }

        const int32 TotalFrames = DataAsset->BakeEndFrame - DataAsset->BakeStartFrame;
        DataAsset->DeltaPos.Empty();
        
        if (TotalFrames < 0) return;

        FVector2D LastFramePos = FVector2D::ZeroVector;
        DataAsset->DeltaPos.Add(FVector2D::ZeroVector); // Frame 0 delta

        // 2. Step through the Bake timeline
        for (int32 i = 1; i <= TotalFrames; ++i)
        {
            const int32 CurrentBakeFrame = DataAsset->BakeStartFrame + i;

            // --- Handle X Axis ---
            float CurrentXValue = 0.0f;
            if (bHasX && CurrentBakeFrame >= DataAsset->X.StartFrame && CurrentBakeFrame <= DataAsset->X.EndFrame)
            {
                int32 XSpan = DataAsset->X.EndFrame - DataAsset->X.StartFrame;
                float AlphaX = (XSpan > 0) ? (static_cast<float>(CurrentBakeFrame - DataAsset->X.StartFrame) / XSpan) : 1.0f;
                CurrentXValue = DataAsset->X.Curve->GetFloatValue(AlphaX) * DataAsset->X.TotalDisplacement;
            }
            else if (bHasX && CurrentBakeFrame > DataAsset->X.EndFrame)
            {
                // If we are past the X movement but Z is still going, maintain the final X position
                CurrentXValue = DataAsset->X.TotalDisplacement;
            }

            // --- Handle Z Axis ---
            float CurrentZValue = 0.0f;
            if (bHasZ && CurrentBakeFrame >= DataAsset->Z.StartFrame && CurrentBakeFrame <= DataAsset->Z.EndFrame)
            {
                int32 ZSpan = DataAsset->Z.EndFrame - DataAsset->Z.StartFrame;
                float AlphaZ = (ZSpan > 0) ? (static_cast<float>(CurrentBakeFrame - DataAsset->Z.StartFrame) / ZSpan) : 1.0f;
                CurrentZValue = DataAsset->Z.Curve->GetFloatValue(AlphaZ) * DataAsset->Z.TotalDisplacement;
            }
            else if (bHasZ && CurrentBakeFrame > DataAsset->Z.EndFrame)
            {
                // If we are past the Z movement but X is still going, maintain final Z position
                CurrentZValue = DataAsset->Z.TotalDisplacement;
            }

            FVector2D CurrentFramePos = FVector2D(CurrentXValue, CurrentZValue);
            FVector2D Delta = CurrentFramePos - LastFramePos;

            DataAsset->DeltaPos.Add(Delta);
            LastFramePos = CurrentFramePos;
        }

    #if WITH_EDITOR
        DataAsset->MarkPackageDirty();
    #endif
    }
};