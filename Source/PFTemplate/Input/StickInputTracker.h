#pragma once

#include "CoreMinimal.h"
#include "Input.h"

// Tracks stick direction, flicks, and allows capturing direction at a specific moment
struct FStickInputTracker
{
private:
    FStickInt8 PreviousInput = FStickInt8::ZeroVector; // X and Y as int8 (stored in int32 for FIntPoint)
    EStickDirection CurrentDirection = EStickDirection::Neutral;
    EStickDirection CapturedDirection = EStickDirection::Neutral;
    EStickDirection FlickDirection = EStickDirection::Neutral;

    int32 FlickHoldFramesRemaining = 0;
    bool bWasDownJustPressedThisFrame = false;

    // Thresholds in quantized int space (-127..127)
    const int32 FlickThreshold = 102; // ~0.8 in float space
    const int32 MinFrom = 51;         // ~0.4 in float space
    const int32 MaxFlickHoldFrames = 5;

public:
    void Tick(const FStickInt8& StickInput)
    {
        bWasDownJustPressedThisFrame = false; // Reset each frame

        // Detect down press transition
        const bool WasPreviouslyNotDown = PreviousInput.Y > -89; // ~-0.7
        const bool IsCurrentlyDown = StickInput.Y < -89;

        if (WasPreviouslyNotDown && IsCurrentlyDown)
        {
            bWasDownJustPressedThisFrame = true;
        }

        // ---- Flick detection ----
        EStickDirection DetectedFlick = EStickDirection::Neutral;

        if (FMath::Abs(StickInput.X) >= FlickThreshold && FMath::Abs(PreviousInput.X) < MinFrom)
        {
            DetectedFlick = (StickInput.X > 0) ? EStickDirection::Right : EStickDirection::Left;
        }
        else if (FMath::Abs(StickInput.Y) >= FlickThreshold && FMath::Abs(PreviousInput.Y) < MinFrom)
        {
            DetectedFlick = (StickInput.Y > 0) ? EStickDirection::Up : EStickDirection::Down;
        }

        if (DetectedFlick != EStickDirection::Neutral)
        {
            FlickDirection = DetectedFlick;
            FlickHoldFramesRemaining = MaxFlickHoldFrames;
        }
        else if (FlickHoldFramesRemaining > 0)
        {
            if (GetDirectionFromInput(StickInput) == FlickDirection)
            {
                --FlickHoldFramesRemaining;
            }
            else
            {
                ClearFlick();
            }
        }

        CurrentDirection = GetDirectionFromInput(StickInput);
        PreviousInput = StickInput;
    }

    bool IsFlickActive() const { return FlickHoldFramesRemaining > 0; }
    EStickDirection GetCurrentDirection() const { return CurrentDirection; }
    void ClearFlick() { FlickHoldFramesRemaining = 0; FlickDirection = EStickDirection::Neutral; }
    void CaptureStickDir() { CapturedDirection = CurrentDirection; }
    EStickDirection GetCapturedDir() const { return CapturedDirection; }
    FStickInt8 GetPreviousInput() const { return PreviousInput; }
    bool isFlickHorizontal() const
    {
        return IsFlickActive() && (FlickDirection == EStickDirection::Left || FlickDirection == EStickDirection::Right);
    }
    bool WasDownJustPressedThisFrame() const { return bWasDownJustPressedThisFrame; }

private:
    EStickDirection GetDirectionFromInput(const FStickInt8& Input) const
    {
        if (FMath::Abs(Input.X) > FMath::Abs(Input.Y))
        {
            return (Input.X > 0) ? EStickDirection::Right :
                   (Input.X < 0) ? EStickDirection::Left : EStickDirection::Neutral;
        }
        else if (FMath::Abs(Input.Y) > 0)
        {
            return (Input.Y > 0) ? EStickDirection::Up :
                   (Input.Y < 0) ? EStickDirection::Down : EStickDirection::Neutral;
        }
        return EStickDirection::Neutral;
    }
};