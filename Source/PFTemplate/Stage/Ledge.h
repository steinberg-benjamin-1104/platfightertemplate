#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Ledge.generated.h"

UCLASS()
class PFTEMPLATE_API ALedge : public AActor
{
	GENERATED_BODY()

public:
	ALedge();

	// True = right ledge, False = left ledge
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ledge")
	bool IsRight = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Ledge")
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Ledge")
	class USphereComponent* DetectionSphere;
};

