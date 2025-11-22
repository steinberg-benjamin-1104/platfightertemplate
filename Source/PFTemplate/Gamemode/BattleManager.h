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

	virtual void Tick(float DeltaTime) override;

	void StartBattle();

private:
	static constexpr float FixedFrameTime = 1.0f / 60.0f;
	bool bCanTick = false;
	bool bShowDebugInfo = true;
	int32 BattleFrame = 0;
};