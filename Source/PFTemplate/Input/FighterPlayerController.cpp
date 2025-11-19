#include "FighterPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "FighterPawn.h"
#include "SafeMath.h" 

AFighterPlayerController::AFighterPlayerController()
{
    PrimaryActorTick.bCanEverTick = false;   // <-  MANUAL TICK ONLY
    PrimaryActorTick.bStartWithTickEnabled = false;
}

void AFighterPlayerController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    FighterPawn = Cast<AFighterPawn>(InPawn);
    ApplyInputMappingContext();
}

void AFighterPlayerController::ApplyInputMappingContext()
{
    if (ULocalPlayer* LP = GetLocalPlayer())
    {
        if (UEnhancedInputLocalPlayerSubsystem* Sub = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            Sub->ClearAllMappings();
            if (IMC) Sub->AddMappingContext(IMC, 0);
        }
    }
}

void AFighterPlayerController::UpdateInput()
{
    if (!FighterPawn) return;

    static int32 TempFrame = 0; // CHANGE THIS
    FFighterInput In = BuildInput();
    FighterPawn->SubmitInput(In, TempFrame); //input buffer is stored here for some reason?
    ++TempFrame;
}

FFighterInput AFighterPlayerController::BuildInput() const
{
    FFighterInput In;
    In.Stick = ReadStick();

    In.Buttons = 0;
    if (IsPressed(AttackAction))  In.Buttons |= FFighterInput::Attack;
    if (IsPressed(SpecialAction)) In.Buttons |= FFighterInput::Special;
    if (IsPressed(ShieldAction))  In.Buttons |= FFighterInput::Shield;
    if (IsPressed(GrabAction))    In.Buttons |= FFighterInput::Grab;
    if (IsPressed(JumpAction))    In.Buttons |= FFighterInput::Jump;
    if (IsPressed(ParryAction))   In.Buttons |= FFighterInput::Parry;
    if (IsPressed(WalkHotkey))    In.Buttons |= FFighterInput::Walk;

    return In;
}

FFixedVector2D AFighterPlayerController::ReadStick() const
{
    FVector2D Raw = GetVec2(MoveAction);
    
    FIXED_32 X = FixedClamp(Raw.X, FIXED_32(-1.f), FIXED_32(1.f));
    FIXED_32 Z = FixedClamp(Raw.Y, FIXED_32(-1.f), FIXED_32(1.f));

    const FIXED_32 Dead(DEAD_ZONE);
    if (X.Abs() < Dead) X = FIXED_32(0);
    if (Z.Abs() < Dead) Z = FIXED_32(0);

    return FFixedVector2D(X, Z);
}

bool AFighterPlayerController::IsPressed(const UInputAction* Action) const
{
    const UEnhancedInputComponent* IC = Cast<UEnhancedInputComponent>(InputComponent);
    if (!IC || !Action)
        return false;

    return IC->GetBoundActionValue(Action).Get<bool>();
}

FVector2D AFighterPlayerController::GetVec2(const UInputAction* Action) const
{
    const UEnhancedInputComponent* IC = Cast<UEnhancedInputComponent>(InputComponent);
    if (!IC || !Action)
        return FVector2D::ZeroVector;

    return IC->GetBoundActionValue(Action).Get<FVector2D>();
}
