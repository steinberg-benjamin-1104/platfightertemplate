// CharacterPanelWidget.cpp
#include "CharacterPanelWidget.h"
#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"

void UCharacterPanelWidget::SetPlayerName(const FString& NewName)
{
	if (PlayerNameText)
	{
		PlayerNameText->SetText(FText::FromString(NewName));
	}
}

void UCharacterPanelWidget::SetDamage(int32 NewDamage)
{
	if (DamageText)
	{
		DamageText->SetText(FText::FromString(FString::Printf(TEXT("Damage: %d"), NewDamage)));
	}
}

void UCharacterPanelWidget::SetStocks(int32 NewStocks)
{
	if (StockText)
	{
		StockText->SetText(FText::FromString(FString::Printf(TEXT("Stocks: %d"), NewStocks)));
	}
}