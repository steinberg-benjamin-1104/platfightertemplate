#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "StickInputTracker.h"
#include "Input.h"

#include "FighterPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class UFighterState;
class AFighterPawn;

UCLASS()
class PFTEMPLATE_API AFighterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AFighterPlayerController();
	void UpdateInput();
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;

	UFUNCTION(BlueprintCallable)
	void ApplyInputMappingContext();

	// Input config
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* IMC;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* AttackAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* SpecialAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* ShieldAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* GrabAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* JumpAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* WalkHotkey;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* ParryAction;

	// Buffer Settings
	UPROPERTY(EditDefaultsOnly)
	int32 MaxBufferSize = 5;

	UPROPERTY(EditDefaultsOnly)
	int32 InputBufferWindow = 5;

	UPROPERTY(EditDefaultsOnly)
	float Deadzone = 0.05f;

	UFUNCTION(BlueprintCallable)
	FStickInt8 GetCurrentStickValue() {return CurrentStickValue;}

	UFUNCTION()
	void UpdateStick();

	bool IsAttackHeld() const {return bHoldingAttack;}
	bool IsShieldHeld() const {return bHoldingShield;}

	bool IsFlickActive() const {return StickTracker.IsFlickActive();}
	EStickDirection GetStickDirection() const {return StickTracker.GetCurrentDirection();}
	
	FStickInputTracker StickTracker;

private:
	// Frame Counter (band-aid for now)
	int32 CurrentFrame = 0;
	bool isWalking = false;
	bool bHoldingAttack = false;
	bool bHoldingShield = false;
	
	// Input
	UPROPERTY() FStickInt8 CurrentStickValue;
	UPROPERTY() TArray<FBufferedInput> InputBuffer;

	// Cached Pawn
	UPROPERTY() APawn* ControlledPawn = nullptr;
	UPROPERTY() AFighterPawn* FighterPawn;


	// Input Handlers
	void HandleStickInput(const FInputActionValue& Value);
	void StickReleased(const FInputActionValue& Value);
	void HandleAttack(const FInputActionValue& Value);
	void HandleAttackReleased(const FInputActionValue& Value);
	void HandleSpecial(const FInputActionValue& Value);
	void HandleShield(const FInputActionValue& Value);
	void ShieldReleased(const FInputActionValue& Value);
	void HandleGrab(const FInputActionValue& Value);
	void HandleJumpPressed(const FInputActionValue& Value);
	void HandleJumpReleased(const FInputActionValue& Value);
	void HandleParry(const FInputActionValue& Value);
	void HandleWalkHotkey(const FInputActionValue& Value) {isWalking = true;}
	void HandleWalkHotkeyReleased(const FInputActionValue& Value) {isWalking = false;}

	// Buffer
	void BufferInput(EInputButtonType ButtonType);
	void ProcessBufferedInputs();
};