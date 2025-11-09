#pragma once

#include "CoreMinimal.h"
#include "FFrameTimer.generated.h"

//Finish on Frame count, so a 4 frame jumpsquat would be framecount = 5
USTRUCT()
struct FFrameTimer
{
    GENERATED_BODY()

    int32 FrameCount = 0;
    int32 CurrentFrame = 0;
    bool bIsRunning = false;
    bool bIsPaused = false;
    bool JustFinishedThisFrame = false;

    TFunction<void(int32 /*CurrentFrame*/)> OnTick;
    TFunction<void()> OnFinish;

    void Start(int32 InFrameCount,
               TFunction<void(int32)> InOnTick = nullptr,
               TFunction<void()> InOnFinish = nullptr)
    {
        FrameCount = InFrameCount;
        CurrentFrame = 1;
        bIsRunning = true;
        bIsPaused = false;
        JustFinishedThisFrame = false;
        OnTick = InOnTick;
        OnFinish = InOnFinish;
    }

    void Reset()
    {
        FrameCount = 0;
        CurrentFrame = 0;
        bIsRunning = false;
        bIsPaused = false;
        JustFinishedThisFrame = false;
        OnTick = nullptr;
        OnFinish = nullptr;
    }

    void Stop() { bIsRunning = false; }

    void Tick()
    {
        JustFinishedThisFrame = false;
        if (!bIsRunning || bIsPaused) return;

        if (OnTick) OnTick(CurrentFrame);

        if (CurrentFrame >= FrameCount)
        {
            JustFinishedThisFrame = true;
            bIsRunning = false;
            if (OnFinish) OnFinish();
        }
        else
        {
            ++CurrentFrame;
        }
    }

    bool IsActive()   const { return bIsRunning && !bIsPaused; }
    bool IsFinished() const { return !bIsRunning && (CurrentFrame >= FrameCount); }
};

USTRUCT()
struct FFrameTimerManager
{
    GENERATED_BODY()

    TMap<FName, FFrameTimer> Timers;
    TSet<FName> FinishedThisTick; // store finished IDs for 1 frame

    void StartTimer(FName ID, int32 FrameCount,
                    TFunction<void(int32)> OnTick = nullptr,
                    TFunction<void()> OnFinish = nullptr)
    {
        FFrameTimer& T = Timers.FindOrAdd(ID);
        T.Start(FrameCount, OnTick, OnFinish);
    }

    void StopTimer(FName ID)
    {
        if (FFrameTimer* T = Timers.Find(ID))
            T->Stop();
    }

    bool IsTimerActive(FName ID) const
    {
        if (const FFrameTimer* T = Timers.Find(ID))
            return T->IsActive();
        return false;
    }

    bool DidTimerJustFinish(FName ID) const
    {
        return FinishedThisTick.Contains(ID);
    }

    void TickAll()
    {
        FinishedThisTick.Reset();
        TArray<FName> ToRemove;

        for (auto& Pair : Timers)
        {
            Pair.Value.Tick();

            if (Pair.Value.JustFinishedThisFrame)
            {
                FinishedThisTick.Add(Pair.Key);
                ToRemove.Add(Pair.Key); // remove after queries
            }
        }

        for (const FName& Key : ToRemove)
        {
            Timers.Remove(Key);
        }
    }
};