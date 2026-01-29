#pragma once

#include "CoreMinimal.h"
#include "BakedSockets.h"
#include "BakedAnimation.generated.h"

UCLASS(BlueprintType)
class UBakedAnimation : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimSequence* AnimSequence;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FBakedSocketTrack> SocketTracks;

	int32 GetNumFrames() const { return SocketTracks[0].Frames.Num(); }

	FBakedSocketKey GetSocketKey(FName SocketName, int32 Frame, bool bMirrored) const;
};