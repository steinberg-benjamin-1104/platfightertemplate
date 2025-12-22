#pragma once

#include "CoreMinimal.h"
#include "BakeSocketsLibrary.generated.h"

class UAnimSequence;
class UBakedAnimation;

UCLASS()
class UBakeSocketsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/* Bake every frame of the supplied animation into a new UBakedAnimation asset. */
	UFUNCTION(BlueprintCallable, Category = "Animation|Sockets", meta = (CallInEditor = "true"))
	static UBakedAnimation* BakeSocketsFromAnimation(
		UAnimSequence* SourceAnimation,
		const TArray<FName>& SocketNames,
		const FString& AssetPath = TEXT("/Game/BakedSockets"),
		const FString& AssetName = TEXT("BA_SocketBaked")
	);
};