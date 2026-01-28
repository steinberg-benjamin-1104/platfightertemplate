#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BakeProfile.generated.h"

UCLASS(BlueprintType)
class UBakeProfile : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	TArray<FName> TargetNames;
};