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

	for (AFighterPawn* F : Fighters) F->UpdateStick();

	for (AFighterPawn* F : Fighters) F->UpdateState();

	for (AFighterPawn* F : Fighters) F->InputPhase();
	
	for (AFighterPawn* F : Fighters) F->UpdateAnimation();
	
	for (AFighterPawn* F : Fighters) F->CollisionPhase();

	for (AFighterPawn* F : Fighters) F->ProcessCollisions();

	for (AFighterPawn* F : Fighters) F->ShieldPhase();

	for (AFighterPawn* F : Fighters) F->FighterDebug();

	BattleCamera->UpdateCamera();
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