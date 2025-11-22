#include "FighterPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "FighterPawn.h"
#include "SafeMath.h" 

AFighterPlayerController::AFighterPlayerController()
{
    PrimaryActorTick.bCanEverTick = false;
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

void AFighterPlayerController::UpdateInput(FFighterInput &NewInput)
{
    InputHistory[0] = NewInput;
    NewInput = BuildInput();
}

FFighterInput AFighterPlayerController::BuildInput()
{
    FFighterInput Out;

    Out.LeftStick.X = FloatToFixed(GetVec2(MoveAction).X);
    Out.LeftStick.Z = FloatToFixed(GetVec2(MoveAction).Y);

    EInputButton Current = EInputButton::None;

    if (IsPressed(AttackAction))  Current |= EInputButton::Attack;
    if (IsPressed(SpecialAction)) Current |= EInputButton::Special;
    if (IsPressed(ShieldAction))  Current |= EInputButton::Shield;

    Out.ButtonsDown = Current;
    Out.ButtonsPressed = Current & ~PrevButtonsDown;

    PrevButtonsDown = Current;

    return Out;
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
    if (!IC || !Action) return false;

    return IC->GetBoundActionValue(Action).Get<bool>();
}

FVector2D AFighterPlayerController::GetVec2(const UInputAction* Action) const
{
    const UEnhancedInputComponent* IC = Cast<UEnhancedInputComponent>(InputComponent);
    if (!IC || !Action) return FVector2D::ZeroVector;

    return IC->GetBoundActionValue(Action).Get<FVector2D>();
}
