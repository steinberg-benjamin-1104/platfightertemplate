#pragma once
#include "CoreMinimal.h"

#include "ObjectPool.generated.h"

class AFighterPawn;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PFTEMPLATE_API UObjectPool : public UActorComponent
{
	GENERATED_BODY()

public:
	
	virtual void Initialize(AFighterPawn* InOwner, int32 PoolSize = 1);
	virtual void FrameStep() {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> PoolClass;

protected:

	UPROPERTY() TArray<AActor*> Pool;
	UPROPERTY() TArray<AActor*> ActivePool;
	
	AActor* RequestObjectFromPool();

	UPROPERTY() AFighterPawn* FighterPawnRef = nullptr;
};