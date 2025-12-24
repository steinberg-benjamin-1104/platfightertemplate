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
        // 1. Basic DataAsset check
        if (!DataAsset)
        {
            UE_LOG(LogTemp, Error, TEXT("BakeMovementData: DataAsset is null."));
            return;
        }

        // 2. Check if BOTH are missing
        const bool bHasX = DataAsset->X.Curve != nullptr;
        const bool bHasZ = DataAsset->Z.Curve != nullptr;

        if (!bHasX && !bHasZ)
        {
            UE_LOG(LogTemp, Warning, TEXT("BakeMovementData: Both X and Z curves are missing for %s. Aborting bake."), *DataAsset->GetName());
            return;
        }

        DataAsset->DeltaPos.Empty();
        
        const int32 TotalFrames = DataAsset->EndFrame - DataAsset->StartFrame;
        if (TotalFrames <= 0) return;

        const float MagX = DataAsset->X.TotalDisplacement;
        const float MagZ = DataAsset->Z.TotalDisplacement;

        FVector2D LastFramePos = FVector2D::ZeroVector;
        DataAsset->DeltaPos.Add(FVector2D::ZeroVector);

        for (int32 i = 1; i <= TotalFrames; ++i)
        {
            const float Alpha = static_cast<float>(i) / static_cast<float>(TotalFrames);

            // 3. Sample safely: Use the curve value if it exists, otherwise 0.0f
            float CurrentX = bHasX ? (DataAsset->X.Curve->GetFloatValue(Alpha) * MagX) : 0.0f;
            float CurrentZ = bHasZ ? (DataAsset->Z.Curve->GetFloatValue(Alpha) * MagZ) : 0.0f;
            
            FVector2D CurrentFramePos = FVector2D(CurrentX, CurrentZ);
            FVector2D Delta = CurrentFramePos - LastFramePos;

            DataAsset->DeltaPos.Add(Delta);
            LastFramePos = CurrentFramePos;
        }

#if WITH_EDITOR
        DataAsset->MarkPackageDirty();
#endif

        UE_LOG(LogTemp, Log, TEXT("BakeMovementData: Successfully baked %d deltas into %s (X: %s, Z: %s)"), 
            DataAsset->DeltaPos.Num(), *DataAsset->GetName(), 
            bHasX ? TEXT("Valid") : TEXT("Missing/Zeroed"), 
            bHasZ ? TEXT("Valid") : TEXT("Missing/Zeroed"));
    }
};