#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GMTest.generated.h"

class ABattleManager;
class APFCamera;
class ASpawnPoints;
class AFighterPawn;
class AFighterPlayerController;

UCLASS()
class PFTEMPLATE_API AGMTest : public AGameModeBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Battle")
	int32 NumPlayers = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Battle")
	int32 NumStocks = 3;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Battle")
	ABattleManager* BattleManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Battle")
	TArray<ASpawnPoints*> SpawnPoints;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Battle")
	TSubclassOf<AFighterPawn> FighterPawnClass;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Battle")
	TArray<AFighterPawn*> FighterPawns;

protected:
	void InitBattle();

	ULocalPlayer* CreateLocalPlayerForIndex(int32 Index);
	AFighterPawn* SpawnAndPossessPawn(APlayerController* PC, int32 SpawnIndex);
	void SetupHUD();
	void AddCharacterPanelForPawn(AFighterPawn* NewPawn, APlayerController* PC, int32 PlayerIndex);
	
	// Widgets
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UHUDWidget> HUDWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UCharacterPanelWidget> CharacterPanelWidgetClass;

	// Runtime reference
	UPROPERTY() UHUDWidget* HUDWidget;
};