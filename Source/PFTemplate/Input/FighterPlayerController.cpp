#include "FighterPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "FighterPawn.h"
#include "FighterState.h"

AFighterPlayerController::AFighterPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AFighterPlayerController::UpdateInput()
{
	CurrentFrame++;
	ProcessBufferedInputs();
}

void AFighterPlayerController::UpdateStick()
{
	StickTracker.Tick(CurrentStickValue);
}


void AFighterPlayerController::ApplyInputMappingContext()
{
	if (ULocalPlayer* LP = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (IMC)
			{
				Subsystem->ClearAllMappings();
				Subsystem->AddMappingContext(IMC, 0);
				UE_LOG(LogTemp, Warning, TEXT("Input mapping context applied."));
			}
		}
	}
}

void AFighterPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ControlledPawn = InPawn;
	FighterPawn = Cast<AFighterPawn>(InPawn);
	ApplyInputMappingContext();
}

void AFighterPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (MoveAction)
			EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFighterPlayerController::HandleStickInput);
			EnhancedInput->BindAction(MoveAction, ETriggerEvent::Completed, this, &AFighterPlayerController::StickReleased);

		if (AttackAction)
			EnhancedInput->BindAction(AttackAction, ETriggerEvent::Started, this, &AFighterPlayerController::HandleAttack);
			EnhancedInput->BindAction(AttackAction, ETriggerEvent::Completed, this, &AFighterPlayerController::HandleAttackReleased);

		if (SpecialAction)
			EnhancedInput->BindAction(SpecialAction, ETriggerEvent::Started, this, &AFighterPlayerController::HandleSpecial);

		if (ShieldAction)
			EnhancedInput->BindAction(ShieldAction, ETriggerEvent::Started, this, &AFighterPlayerController::HandleShield);
			EnhancedInput->BindAction(ShieldAction, ETriggerEvent::Completed, this, &AFighterPlayerController::ShieldReleased);

		if (GrabAction)
			EnhancedInput->BindAction(GrabAction, ETriggerEvent::Started, this, &AFighterPlayerController::HandleGrab);

		if (JumpAction)
			EnhancedInput->BindAction(JumpAction, ETriggerEvent::Started, this, &AFighterPlayerController::HandleJumpPressed);
			EnhancedInput->BindAction(JumpAction, ETriggerEvent::Completed, this, &AFighterPlayerController::HandleJumpReleased);

		if (WalkHotkey)
			EnhancedInput->BindAction(WalkHotkey, ETriggerEvent::Started, this, &AFighterPlayerController::HandleWalkHotkey);
			EnhancedInput->BindAction(WalkHotkey, ETriggerEvent::Completed, this, &AFighterPlayerController::HandleWalkHotkeyReleased);

		if (ParryAction)
			EnhancedInput->BindAction(ParryAction, ETriggerEvent::Started, this, &AFighterPlayerController::HandleParry);
	}
}

static FORCEINLINE int8 QuantizeAxis(float Value, float Deadzone)
{
	if (FMath::Abs(Value) < Deadzone) return 0;
	
	const float Scaled = FMath::Clamp(Value, -1.f, 1.f) * 127.f;
	return static_cast<int8>(FMath::RoundHalfToZero(Scaled));
}

void AFighterPlayerController::HandleStickInput(const FInputActionValue& Value)
{
	FVector2D StickValue = Value.Get<FVector2D>();

	int8 QuantizedX = QuantizeAxis(StickValue.X, Deadzone);
	int8 QuantizedY = QuantizeAxis(StickValue.Y, Deadzone);

	if (isWalking)
	{
		// Lock walking to discrete step
		QuantizedX = (QuantizedX > 0) ? 64 :
					 (QuantizedX < 0) ? -64 : 0;
		QuantizedY = (QuantizedY > 0) ? 64 :
					 (QuantizedY < 0) ? -64 : 0;
	}
	
	CurrentStickValue = FStickInt8(QuantizedX, QuantizedY);
}

void AFighterPlayerController::StickReleased(const FInputActionValue& Value)
{
	CurrentStickValue = FStickInt8::ZeroVector;
}

void AFighterPlayerController::HandleAttack(const FInputActionValue& Value)
{
	BufferInput(EInputButtonType::Attack);
	bHoldingAttack = true;
}

void AFighterPlayerController::HandleAttackReleased(const FInputActionValue& Value)
{
	if (!FighterPawn) return;
	FighterPawn->StateMachine->AttackReleased();
	bHoldingAttack = false;
}

void AFighterPlayerController::HandleSpecial(const FInputActionValue& Value)
{
	BufferInput(EInputButtonType::Special);
}

void AFighterPlayerController::HandleShield(const FInputActionValue& Value)
{
	BufferInput(EInputButtonType::Shield);
	bHoldingShield = true;
}

void AFighterPlayerController::HandleGrab(const FInputActionValue& Value)
{
	BufferInput(EInputButtonType::Grab);
}

void AFighterPlayerController::HandleJumpPressed(const FInputActionValue& Value)
{
	BufferInput(EInputButtonType::Jump);
}

void AFighterPlayerController::HandleJumpReleased(const FInputActionValue& Value)
{
	if (!FighterPawn) return;
	FighterPawn->StateMachine->JumpReleased();
}

void AFighterPlayerController::ShieldReleased(const FInputActionValue& Value)
{
	if (!FighterPawn) return;
	FighterPawn->StateMachine->ShieldReleased();
	bHoldingShield = false;
}

void AFighterPlayerController::HandleParry(const FInputActionValue& Value)
{
	if (!FighterPawn) return;
	FighterPawn->StateMachine->Parry();
}

void AFighterPlayerController::BufferInput(EInputButtonType ButtonType)
{
	for (const FBufferedInput& Existing : InputBuffer)
	{
		if (Existing.Button == ButtonType)
			return;
	}

	FBufferedInput NewInput;
	NewInput.Button = ButtonType;
	NewInput.StickInput = CurrentStickValue;
	NewInput.FrameNumber = CurrentFrame;

	InputBuffer.Add(NewInput);

	if (InputBuffer.Num() > MaxBufferSize)
	{
		InputBuffer.RemoveAt(0);
	}
}

void AFighterPlayerController::ProcessBufferedInputs()
{
	if (!FighterPawn) return;

	UFighterStateMachine* StateMachine = FighterPawn->StateMachine;
	if (!StateMachine || !StateMachine->CurrentState) return;

	for (int32 i = InputBuffer.Num() - 1; i >= 0; i--)
	{
		FBufferedInput& Input = InputBuffer[i];

		// Expire old inputs
		if (CurrentFrame - Input.FrameNumber > InputBufferWindow)
		{
			InputBuffer.RemoveAt(i);
			continue;
		}

		bool bSuccess = false;

		switch (Input.Button)
		{
			case EInputButtonType::Attack:
				bSuccess = StateMachine->Attack();
				break;
			case EInputButtonType::Special:
				bSuccess = StateMachine->Special();
				break;
			case EInputButtonType::Jump:
				bSuccess = StateMachine->JumpPressed();
				break;
			case EInputButtonType::Shield:
				bSuccess = StateMachine->Shield();
				break;
			case EInputButtonType::Grab:
				bSuccess = StateMachine->Grab();
				break;
			default:
				break;
		}

		if (bSuccess)
		{
			InputBuffer.RemoveAt(i);
		}
	}
}