#include "HUDWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "CharacterPanelWidget.h"

void UHUDWidget::AddPlayerPanel(int32 PlayerIndex)
{
	if (!PlayerGrid) return;

	// Spawn a panel widget (assuming you made a BP subclass of UCharacterPanelWidget)
	UCharacterPanelWidget* NewPanel =
		CreateWidget<UCharacterPanelWidget>(GetWorld(), UCharacterPanelWidget::StaticClass());

	if (NewPanel)
	{
		UUniformGridSlot* Slot = PlayerGrid->AddChildToUniformGrid(NewPanel, 0, PlayerIndex);
		PlayerPanels.Insert(NewPanel, PlayerIndex);
	}
}

UCharacterPanelWidget* UHUDWidget::GetPanelForPlayer(int32 PlayerIndex) const
{
	return PlayerPanels.IsValidIndex(PlayerIndex) ? PlayerPanels[PlayerIndex] : nullptr;
}