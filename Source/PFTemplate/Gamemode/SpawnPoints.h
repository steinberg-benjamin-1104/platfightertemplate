#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BillboardComponent.h"
#include "SpawnPoints.generated.h"

UCLASS(Blueprintable)
class PFTEMPLATE_API ASpawnPoints : public AActor
{
	GENERATED_BODY()

public:
	ASpawnPoints();
	
#if WITH_EDITOR
    virtual void OnConstruction(const FTransform& Transform) override;
#endif

protected:

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UBillboardComponent* Billboard;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    class UArrowComponent* FacingArrow;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawn")
	int32 PlayerIndex = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawn")
    bool bFacingRight = true;
    
private:
    float height = 100.f;
};