#pragma once
#include "CoreMinimal.h"
#include "SafeMath.h"
#include "FighterInput.h"
#include "StickTracker.generated.h"

UENUM()
enum class EStickDirection : uint8
{
    Neutral, Up, Down, Left, Right
};

USTRUCT()
struct FStickTracker
{
    GENERATED_BODY()

    UPROPERTY()
    FFixedVector2D Prev{ fixed32::MakeZero(), fixed32::MakeZero() };

    UPROPERTY()
    EStickDirection CurDir   = EStickDirection::Neutral;
    UPROPERTY()
    EStickDirection Captured = EStickDirection::Neutral;
    UPROPERTY()
    EStickDirection FlickDir = EStickDirection::Neutral;
    UPROPERTY()
    int32           FlickLeft= 0;

    static constexpr int32 MAX_FLICK_HOLD = 5;

    void Tick(const FFixedVector2D& Raw, bool bWalk)
    {
        // apply walk filter first
        FFixedVector2D Stick = Raw;
        if (bWalk)
        {
            Stick.X = (Stick.X >  dead_zone) ?  fixed32::Make(0.5f) :
                      (Stick.X < -dead_zone) ? -fixed32::Make(0.5f) : fixed32::MakeZero();
            Stick.Z = (Stick.Z >  dead_zone) ?  fixed32::Make(0.5f) :
                      (Stick.Z < -dead_zone) ? -fixed32::Make(0.5f) : fixed32::MakeZero();
        }

        // down-press detection
        const bool wasDown = Prev.Z < -fixed32::Make(0.7f);
        const bool nowDown = Stick.Z < -fixed32::Make(0.7f);
        bDownJustPressed = (!wasDown && nowDown);

        // flick
        EStickDirection detected = EStickDirection::Neutral;
        if (Stick.X.Abs() >= flick_max && Prev.X.Abs() < flick_min)
            detected = (Stick.X > 0) ? EStickDirection::Right : EStickDirection::Left;
        else if (Stick.Z.Abs() >= flick_max && Prev.Z.Abs() < flick_min)
            detected = (Stick.Z > 0) ? EStickDirection::Up    : EStickDirection::Down;

        if (detected != EStickDirection::Neutral)
        {
            FlickDir   = detected;
            FlickLeft  = MAX_FLICK_HOLD;
        }
        else if (FlickLeft > 0)
        {
            if (GetDir(Stick) == FlickDir) --FlickLeft;
            else                             ClearFlick();
        }

        CurDir = GetDir(Stick);
        Prev   = Stick;
    }

    void ClearFlick()            { FlickLeft = 0; FlickDir = EStickDirection::Neutral; }
    void Capture()               { Captured = CurDir; }
    bool IsFlickActive()  const  { return FlickLeft > 0; }
    bool IsFlickHoriz()   const  { return IsFlickActive() && (FlickDir==EStickDirection::Left||FlickDir==EStickDirection::Right); }
    bool WasDownJustPressed() const{ return bDownJustPressed; }
    EStickDirection GetCurrent()const{ return CurDir; }
    EStickDirection GetCaptured()const{ return Captured; }

private:
    bool bDownJustPressed = false;
    EStickDirection GetDir(const FFixedVector2D& V) const
    {
        if (V.X.Abs() > V.Z.Abs())
            return V.X > 0 ? EStickDirection::Right : EStickDirection::Left;
        if (V.Z.Abs() > dead_zone)
            return V.Z > 0 ? EStickDirection::Up : EStickDirection::Down;
        return EStickDirection::Neutral;
    }
    static constexpr fixed32 dead_zone = DEAD_ZONE;
    static constexpr fixed32 flick_min = FLICK_MIN;
    static constexpr fixed32 flick_max = FLICK_MAX;
};