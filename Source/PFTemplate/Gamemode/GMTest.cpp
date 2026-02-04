#include "GMTest.h"
#include "BattleManager.h"
#include "SpawnPoints.h"
#include "FighterPawn.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "PFCamera.h"
#include "CharacterPanelWidget.h"
#include "HUDWidget.h"
#include "Components/UniformGridSlot.h"
#include "Components/UniformGridPanel.h"

void AGMTest::BeginPlay()
{
	Super::BeginPlay();
	
	if (!BattleManager)
	{
		TArray<AActor*> FoundManagers;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABattleManager::StaticClass(), FoundManagers);
		if (FoundManagers.Num() > 0)
			BattleManager = Cast<ABattleManager>(FoundManagers[0]);
	}

	if (SpawnPoints.Num() == 0)
	{
		TArray<AActor*> FoundSpawns;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnPoints::StaticClass(), FoundSpawns);
		for (AActor* SpawnActor : FoundSpawns)
		{
			if (ASpawnPoints* Spawn = Cast<ASpawnPoints>(SpawnActor))
			{
				SpawnPoints.Add(Spawn);
			}
		}
		// Sort by PlayerIndex if desired
		SpawnPoints.Sort([](const ASpawnPoints& A, const ASpawnPoints& B) {
			return A.PlayerIndex < B.PlayerIndex;
		});
	}

	InitBattle();
}

void AGMTest::InitBattle()
{
	if (!FighterPawnClass || SpawnPoints.Num() == 0 || !GetWorld()) return;

	SetupHUD();
	
	for (int32 i = 0; i < NumPlayers; i++)
	{
		ULocalPlayer* LocalPlayer = CreateLocalPlayerForIndex(i);
		if (!LocalPlayer) continue;

		APlayerController* PC = LocalPlayer->PlayerController;
		if (!PC) continue;

		AFighterPawn* NewPawn = SpawnAndPossessPawn(PC, i);
		if (NewPawn)
		{
			AddCharacterPanelForPawn(NewPawn, PC, i);
		}
	}

	if (BattleManager) BattleManager->StartBattle();
}

ULocalPlayer* AGMTest::CreateLocalPlayerForIndex(int32 Index)
{
	if (Index == 0)
	{
		return GetGameInstance()->GetFirstGamePlayer();
	}

	FString OutError;
	FPlatformUserId UserId = FGenericPlatformMisc::GetPlatformUserForUserIndex(Index);
	ULocalPlayer* LocalPlayer = GetGameInstance()->CreateLocalPlayer(UserId, OutError, true);

	if (!OutError.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create local player %d: %s"), Index, *OutError);
	}

	return LocalPlayer;
}

AFighterPawn* AGMTest::SpawnAndPossessPawn(APlayerController* PC, int32 SpawnIndex)
{
	if (!PC) return nullptr;

	UWorld* World = GetWorld();
	if (!World) return nullptr;

	ASpawnPoints* Spawn = (SpawnIndex < SpawnPoints.Num()) ? SpawnPoints[SpawnIndex] : nullptr;
	FVector SpawnLoc = Spawn ? Spawn->GetActorLocation() : FVector::ZeroVector;
	FRotator SpawnRot = FRotator::ZeroRotator;

	AFighterPawn* NewPawn = World->SpawnActor<AFighterPawn>(FighterPawnClass, SpawnLoc, SpawnRot);
	if (NewPawn)
	{
		PC->Possess(NewPawn);
		FighterPawns.Add(NewPawn);
		NewPawn->PlayerID = SpawnIndex;

		if (BattleManager)
		{
			BattleManager->BattleCamera->AddTargetActor(NewPawn);
			BattleManager->Fighters.Add(NewPawn);
		}

		if (Spawn) NewPawn->SetFacingDirection(Spawn->bFacingRight ? 1.0f : -1.0f);
	}

	return NewPawn;
}

void AGMTest::SetupHUD()
{
	if (!HUDWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("No HUD class declared"));
		return;
	}
	HUDWidget = CreateWidget<UHUDWidget>(GetWorld(), HUDWidgetClass);
	HUDWidget->AddToViewport();
}

void AGMTest::AddCharacterPanelForPawn(AFighterPawn* NewPawn, APlayerController* PC, int32 PlayerIndex)
{
	if (!HUDWidget || !CharacterPanelWidgetClass) return;

	if (UUniformGridPanel* PlayerGrid = HUDWidget->PlayerGrid)
	{
		UCharacterPanelWidget* Panel = CreateWidget<UCharacterPanelWidget>(PC, CharacterPanelWidgetClass);
		Panel->SetPlayerName(
		FString::Printf(TEXT("Player %d"), PlayerIndex + 1)
		);
		Panel->SetStocks(NumStocks);

		if (!Panel) return;

		// Give pawn reference to its panel
		NewPawn->SetCharacterPanel(Panel);

		// Add to grid
		UUniformGridSlot* GridSlot = PlayerGrid->AddChildToUniformGrid(Panel, 0, PlayerIndex);
		if (GridSlot)
		{
			GridSlot->SetHorizontalAlignment(HAlign_Center);
			GridSlot->SetVerticalAlignment(VAlign_Center);
		}
	}
}