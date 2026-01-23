#include "BattleManager.h"
#include "FighterPawn.h"
#include "PFCamera.h"
#include "RollbackSimSubsystem.h"

ABattleManager::ABattleManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABattleManager::BeginPlay()
{
	Super::BeginPlay();

	if (UGameInstance* GI = GetGameInstance())
	{
		if (URollbackSimSubsystem* Sim = GI->GetSubsystem<URollbackSimSubsystem>())
		{
			Sim->RegisterBattleManager(this);
		}
	}
}

void ABattleManager::StepFrame()
{
	if (!bCanUpdate) return;
	
	//Update States, Animation, Movement
	for (AFighterPawn* F : Fighters) F->PreCollisionPhase(BattleFrame);

	//Anything Relying on position/anim of another fighter
	for (AFighterPawn* F: Fighters) F->UpdateDependencies();
	
	//Test for Collision After Char Updates
	for (AFighterPawn* F : Fighters) F->DetectCollisions();

	//Update States, Animation, Movement per collision
	for (AFighterPawn* F : Fighters) F->ProcessCollisions();

	for (AFighterPawn* F : Fighters) F->PostCollisionPhase();

	for (AFighterPawn* F : Fighters) F->FighterDebug();

	BattleCamera->UpdateCamera();

	BattleFrame++;
}

void ABattleManager::StartBattle()
{
	bCanUpdate = true;
	
	if (Fighters.Num() > 0 && BattleCamera)
	{
		if (APlayerController* PC0 = Cast<APlayerController>(Fighters[0]->GetController()))
		{
			PC0->SetViewTargetWithBlend(BattleCamera, 0.f);
		}
	}
}