#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "BattleManager.generated.h"

class AFighterPawn;
class APFCamera;

UCLASS()
class PFTEMPLATE_API ABattleManager : public AActor
{
	GENERATED_BODY()

public:
	ABattleManager();
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<AFighterPawn*> Fighters;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Battle")
	APFCamera* BattleCamera;

	void StepFrame();

	void StartBattle();

protected:
	virtual void BeginPlay() override;
	
private:
	bool bCanUpdate = false;
	bool bShowDebugInfo = true;
	int32 BattleFrame = 0;
};