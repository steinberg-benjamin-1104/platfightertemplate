// CharacterPanelWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterPanelWidget.generated.h"

UCLASS()
class PFTEMPLATE_API UCharacterPanelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Update HUD elements from code
	UFUNCTION(BlueprintCallable)
	void SetPlayerName(const FString& NewName);

	UFUNCTION(BlueprintCallable)
	void SetDamage(int32 NewPercent);

	UFUNCTION(BlueprintCallable)
	void SetStocks(int32 NewStocks);

protected:
	// Bind these to the widgets in your .uasset (UMG designer)
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PlayerNameText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DamageText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* StockText;
};