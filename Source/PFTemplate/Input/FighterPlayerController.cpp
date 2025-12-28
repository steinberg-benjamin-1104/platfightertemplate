#include "FighterPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "SafeMath.h"
#include "InputMappingContext.h"

void AFighterPlayerController::UpdateInput(int32 Frame, FFighterInput& NewInput)
{
    NewInput = BuildInput();
}

FFighterInput AFighterPlayerController::BuildInput()
{
    FFighterInput Out;
    
    uint16 Current = 0;

    if (IsPressed(AttackAction)) Current |= static_cast<uint32>(EInputButton::Attack);
    if (IsPressed(SpecialAction)) Current |= static_cast<uint32>(EInputButton::Special);
    if (IsPressed(ShieldAction)) Current |= static_cast<uint32>(EInputButton::Shield);
    if (IsPressed(JumpAction)) Current |= static_cast<uint32>(EInputButton::Jump);
    if (IsPressed(ParryAction)) Current |= static_cast<uint32>(EInputButton::Parry);
    if (IsPressed(GrabAction)) Current |= static_cast<uint32>(EInputButton::Grab);

    Out.Held     = Current;
    Out.Pressed  = Current & ~PrevButtonsDown;
    
    UpdateStickState(Out, ReadStick(IsPressed(WalkHotkey)));
    
    PrevButtonsDown = Current;

    return Out;
}

FFixedVector2D AFighterPlayerController::ReadStick(bool bReduceRadius) const
{
    FVector2D Raw = GetVec2(MoveAction);

    float radius = bReduceRadius ? 0.5f : 1.f;
    FFixed_32 X = FixedClamp(Raw.X, FFixed_32(-radius), FFixed_32(radius));
    FFixed_32 Z = FixedClamp(Raw.Y, FFixed_32(-radius), FFixed_32(radius));

    return FFixedVector2D(X, Z);
}

bool AFighterPlayerController::IsPressed(const UInputAction* Action) const
{
    const UEnhancedPlayerInput* EPI = Cast<UEnhancedPlayerInput>(PlayerInput);

    if (!EPI || !Action) return false;

    return EPI->GetActionValue(Action).Get<bool>();
}

FVector2D AFighterPlayerController::GetVec2(const UInputAction* Action) const
{
    const UEnhancedPlayerInput* EPI = Cast<UEnhancedPlayerInput>(PlayerInput);

    if (!EPI || !Action) return FVector2D::ZeroVector;

    return EPI->GetActionValue(Action).Get<FVector2D>();
}

void AFighterPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    
    if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent))
    {
        if (MoveAction) EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFighterPlayerController::DummyAction);
        if (AttackAction) EIC->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AFighterPlayerController::DummyAction);
        if (SpecialAction) EIC->BindAction(SpecialAction, ETriggerEvent::Triggered, this, &AFighterPlayerController::DummyAction);
        if (ShieldAction) EIC->BindAction(ShieldAction, ETriggerEvent::Triggered, this, &AFighterPlayerController::DummyAction);
    }
}

void AFighterPlayerController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    if (ULocalPlayer* LP = GetLocalPlayer())
    {
        if (UEnhancedInputLocalPlayerSubsystem* SubSys = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            if (IMC) SubSys->AddMappingContext(IMC, 0);
        }
    }
}

void AFighterPlayerController::UpdateStickState(FFighterInput& InputState, FFixedVector2D New)
{
    if (New.X.Abs() < DeadZone) New.X = FFixed_32(0.f);
    if (New.Z.Abs() < DeadZone) New.Z = FFixed_32(0.f);
	
    InputState.StickPos = New;
	
    if ((InputState.StickPos.Z < -DownThreshold) && !(PrevStick.Z < -DownThreshold))
    {
        InputState.Pressed |= static_cast<uint32>(EInputButton::StickDown);
    }

    auto FlickAxis = [&](FFixed_32 Curr, FFixed_32 Prev)
    {
        const bool bCurrStrong = Curr.Abs() >= FlickEnd;
        const bool bPrevNeutral = Prev.Abs() <= FlickStart;

        const bool bSignFlip =
            Prev.Abs() >= FlickEnd &&
            ((Curr > FFixed_32(0)) != (Prev > FFixed_32(0)));

        return bCurrStrong && (bPrevNeutral || bSignFlip);
    };

    bool FlickX = FlickAxis(InputState.StickPos.X, PrevStick.X);
    bool FlickY = FlickAxis(InputState.StickPos.Z, PrevStick.Z);
	
    if (FlickX || FlickY) InputState.Pressed |=static_cast<uint32>(EInputButton::Flick);
    PrevStick = New;
}