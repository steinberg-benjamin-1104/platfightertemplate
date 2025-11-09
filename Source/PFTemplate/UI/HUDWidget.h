#pragma once

#include "Blueprint/UserWidget.h"
#include "HUDWidget.generated.h"

class UUniformGridPanel;
class UCharacterPanelWidget;

UCLASS()
class PFTEMPLATE_API UHUDWidget : public UUserWidget
{
	GENERATED_BODY()

	public:
	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* PlayerGrid;

	UFUNCTION(BlueprintCallable)
	void AddPlayerPanel(int32 PlayerIndex);

	UCharacterPanelWidget* GetPanelForPlayer(int32 PlayerIndex) const;

	private:
	UPROPERTY()
	TArray<UCharacterPanelWidget*> PlayerPanels;
};