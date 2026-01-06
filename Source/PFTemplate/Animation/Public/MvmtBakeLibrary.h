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

        // 1. Calculate the inclusive Bake bounds
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

        // Calculate total number of frames to process (e.g., 0 to 9 is 10 frames)
        const int32 FrameCount = (DataAsset->BakeEndFrame - DataAsset->BakeStartFrame) + 1;
        if (FrameCount <= 0) return;

        DataAsset->DeltaPos.Empty();
        
        // Tracks the position reached at the end of the previous iteration
        FVector2D LastFramePos = FVector2D::ZeroVector;

        // 2. Step through the Bake timeline
        for (int32 i = 0; i < FrameCount; ++i)
        {
            const int32 CurrentBakeFrame = DataAsset->BakeStartFrame + i;

            // --- Handle X Axis ---
            float CurrentXValue = 0.0f;
            if (bHasX)
            {
                if (CurrentBakeFrame >= DataAsset->X.StartFrame && CurrentBakeFrame <= DataAsset->X.EndFrame)
                {
                    // Duration is inclusive (End - Start + 1)
                    int32 XDuration = (DataAsset->X.EndFrame - DataAsset->X.StartFrame) + 1;
                    // Current step is 1-based to capture the movement through the end of the frame
                    int32 CurrentStepX = (CurrentBakeFrame - DataAsset->X.StartFrame) + 1;
                    
                    float AlphaX = static_cast<float>(CurrentStepX) / XDuration;
                    CurrentXValue = DataAsset->X.Curve->GetFloatValue(AlphaX) * DataAsset->X.TotalDisplacement;
                }
                else if (CurrentBakeFrame > DataAsset->X.EndFrame)
                {
                    // Maintain final position if X finishes before Z
                    CurrentXValue = DataAsset->X.TotalDisplacement;
                }
            }

            // --- Handle Z Axis ---
            float CurrentZValue = 0.0f;
            if (bHasZ)
            {
                if (CurrentBakeFrame >= DataAsset->Z.StartFrame && CurrentBakeFrame <= DataAsset->Z.EndFrame)
                {
                    int32 ZDuration = (DataAsset->Z.EndFrame - DataAsset->Z.StartFrame) + 1;
                    int32 CurrentStepZ = (CurrentBakeFrame - DataAsset->Z.StartFrame) + 1;
                    
                    float AlphaZ = static_cast<float>(CurrentStepZ) / ZDuration;
                    CurrentZValue = DataAsset->Z.Curve->GetFloatValue(AlphaZ) * DataAsset->Z.TotalDisplacement;
                }
                else if (CurrentBakeFrame > DataAsset->Z.EndFrame)
                {
                    // Maintain final position if Z finishes before X
                    CurrentZValue = DataAsset->Z.TotalDisplacement;
                }
            }

            // Calculate Delta for this frame
            FVector2D CurrentFramePos = FVector2D(CurrentXValue, CurrentZValue);
            FVector2D Delta = CurrentFramePos - LastFramePos;

            DataAsset->DeltaPos.Add(Delta);
            
            // Update LastFramePos for the next iteration
            LastFramePos = CurrentFramePos;
        }

    #if WITH_EDITOR
        DataAsset->MarkPackageDirty();
    #endif
    }
};