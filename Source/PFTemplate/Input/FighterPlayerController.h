#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FighterInput.h"
#include "FighterPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class AFighterPawn;

UCLASS()
class PFTEMPLATE_API AFighterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AFighterPlayerController();
	void UpdateInput(FFighterInput &NewInput);

protected:
	virtual void OnPossess(APawn* InPawn) override;
	
	UFUNCTION(BlueprintCallable, Category = "Input")
	void ApplyInputMappingContext();
	
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
	FFixedVector2D ReadStick() const;

	bool IsPressed(const UInputAction* Action) const;
	FVector2D GetVec2(const UInputAction* Action) const;

private:
	/* cached pawn (cast once) */
	UPROPERTY()
	AFighterPawn* FighterPawn = nullptr;

	float DEAD_ZONE = 0.1f;

	TArray<FFighterInput> InputHistory;
	EInputButton PrevButtonsDown = EInputButton::None;
};