#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoxBounds.h"
#include "Stage.generated.h"

UCLASS()
class PFTEMPLATE_API AStage : public AActor
{
	GENERATED_BODY()

public:
	AStage();

protected:
	virtual void Tick(float DeltaSeconds) override;

public:

	UPROPERTY(EditAnywhere, Category = "Blastzone")
	FBoxBounds Blastzone;

	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bDrawDebug = true;


private:
	void DrawBlastzone(UWorld* World);
};