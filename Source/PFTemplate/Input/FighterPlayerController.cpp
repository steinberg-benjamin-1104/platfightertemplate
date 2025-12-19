#include "FighterPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "SafeMath.h"
#include "InputMappingContext.h"
#include "Containers/Array.h"

void AFighterPlayerController::UpdateInput(int32 Frame, FFighterInput& NewInput)
{
    NewInput = BuildInput();
}

FFighterInput AFighterPlayerController::BuildInput()
{
    FFighterInput Out;
    
    const FFixedVector2D Stick = ReadStick();
    UpdateStickState(Out.Stick, Stick, PrevStick);
    
    uint16 Current = 0;

    if (IsPressed(AttackAction)) Current |= static_cast<uint32>(EInputButton::Attack);
    if (IsPressed(SpecialAction)) Current |= static_cast<uint32>(EInputButton::Special);
    if (IsPressed(ShieldAction)) Current |= static_cast<uint32>(EInputButton::Shield);
    if (IsPressed(JumpAction)) Current |= static_cast<uint32>(EInputButton::Jump);
    if (IsPressed(ParryAction)) Current |= static_cast<uint32>(EInputButton::Parry);
    if (IsPressed(GrabAction)) Current |= static_cast<uint32>(EInputButton::Grab);

    Out.Button.Down     = Current;
    Out.Button.Pressed  = Current & ~PrevButtonsDown;

    PrevButtonsDown = Current;
    PrevStick = Stick;

    return Out;
}

FFixedVector2D AFighterPlayerController::ReadStick() const
{
    FVector2D Raw = GetVec2(MoveAction);
    
    FFixed_32 X = FixedClamp(Raw.X, FFixed_32(-1.f), FFixed_32(1.f));
    FFixed_32 Z = FixedClamp(Raw.Y, FFixed_32(-1.f), FFixed_32(1.f));

    const FFixed_32 Dead(DEAD_ZONE);
    if (X.Abs() < Dead) X = FFixed_32(0);
    if (Z.Abs() < Dead) Z = FFixed_32(0);

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