#include "BattleManager.h"
#include "FighterPawn.h"
#include "PFCamera.h"

ABattleManager::ABattleManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABattleManager::Tick(float DeltaTime)
{
	if (!bCanTick) return;
	
	
	for (AFighterPawn* F : Fighters) F->PreCollisionPhase(BattleFrame);

	for (AFighterPawn* F : Fighters) F->ProcessCollisions();

	for (AFighterPawn* F : Fighters) F->PostCollisionPhase();

	for (AFighterPawn* F : Fighters) F->FighterDebug();

	BattleCamera->UpdateCamera();

	BattleFrame++;
}

void ABattleManager::StartBattle()
{
	bCanTick = true;
	
	if (Fighters.Num() > 0 && BattleCamera)
	{
		if (APlayerController* PC0 = Cast<APlayerController>(Fighters[0]->GetController()))
		{
			PC0->SetViewTargetWithBlend(BattleCamera, 0.f);
		}
	}
}