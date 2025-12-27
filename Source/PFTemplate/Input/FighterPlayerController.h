#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FighterInput.h"
#include "InputActionValue.h"
#include "FighterPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS()
class PFTEMPLATE_API AFighterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	void UpdateInput(int32 Frame, );

	void DummyAction(const FInputActionValue&){}

protected:
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;

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
	
	FFighterInput BuildInput();
	FFixedVector2D ReadStick(bool bSmallClamp) const;

	bool IsPressed(const UInputAction* Action) const;
	FVector2D GetVec2(const UInputAction* Action) const;

private:

	float DEAD_ZONE = 0.1f;
	
	uint16 PrevButtonsDown;
	FFixedVector2D PrevStick;

};