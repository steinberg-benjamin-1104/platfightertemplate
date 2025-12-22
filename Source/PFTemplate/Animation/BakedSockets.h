#pragma once

#include "BakedSockets.generated.h"

USTRUCT(BlueprintType)
struct FBakedSocketTransform
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector Location;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FRotator Rotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector Scale;
};

USTRUCT(BlueprintType)
struct FBakedSocketTrack
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FName SocketName;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FBakedSocketTransform> Frames;
};

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

	bool GetSocketTransform(FName SocketName, int32 Frame, bool bMirrored, FTransform& OutTransform) const;
};