#include "BakeSocketsLibrary.h"
#include "Animation/AnimSequence.h"
#include "Animation/Skeleton.h"
#include "Engine/SkeletalMeshSocket.h"
#include "AssetToolsModule.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "BonePose.h"
#include "BakedSockets.h"

// Helper to calculate Component Space transforms from the Animation
void GetComponentSpacePoseAtTime(const UAnimSequence* AnimSequence, double Time, FBoneContainer& BoneContainer, FCSPose<FCompactPose>& OutCSPose)
{
    // 1. Initialize the required data structures
    FCompactPose Pose;
    Pose.SetBoneContainer(&BoneContainer);

    FBlendedCurve Curve;
    Curve.InitFrom(BoneContainer);

    UE::Anim::FStackAttributeContainer Attributes;

    // 2. Bundle them into FAnimationPoseData
    // This wrapper struct is required by the modern GetBonePose API
    FAnimationPoseData AnimationPoseData(Pose, Curve, Attributes);

    // 3. Create the extraction context
    // This handles the time sampling and loop settings
    FAnimExtractContext Context(Time, AnimSequence->bLoop);
    
    // 4. Extract the local bone pose using the new signature
    AnimSequence->GetBonePose(AnimationPoseData, Context);

    // 5. Convert to Component Space (Global Pose relative to root)
    OutCSPose.InitPose(Pose);
}

UBakedAnimation* UBakeSocketsLibrary::BakeSocketsFromAnimation(
    UAnimSequence* SourceAnimation,
    const TArray<FName>& SocketNames,
    const FString& AssetPath,
    const FString& AssetName)
{
#if WITH_EDITOR
    if (!SourceAnimation)
    {
        UE_LOG(LogTemp, Error, TEXT("BakeSockets: Source Animation is null."));
        return nullptr;
    }

    USkeleton* Skeleton = SourceAnimation->GetSkeleton();
    if (!Skeleton)
    {
        UE_LOG(LogTemp, Error, TEXT("BakeSockets: Source Animation has no Skeleton."));
        return nullptr;
    }

    if (SocketNames.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("BakeSockets: No socket names provided."));
        return nullptr;
    }

    // --- 1. Create the New Asset ---
    IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
    
    FString PackageName;
    FString FinalAssetName;
    AssetTools.CreateUniqueAssetName(AssetPath / AssetName, TEXT(""), PackageName, FinalAssetName);

    UPackage* Package = CreatePackage(*PackageName);
    
    UBakedAnimation* NewBakedData = NewObject<UBakedAnimation>(
        Package, 
        UBakedAnimation::StaticClass(), 
        *FinalAssetName, 
        RF_Public | RF_Standalone
    );

    if (!NewBakedData)
    {
        UE_LOG(LogTemp, Error, TEXT("BakeSockets: Failed to create asset."));
        return nullptr;
    }

    NewBakedData->AnimSequence = SourceAnimation;
    NewBakedData->SocketTracks.Empty();

    // --- 2. Initialize Animation Evaluation Context ---
    // We need a BoneContainer to process poses.
    // We will include all bones in the reference skeleton to ensure hierarchy calculations are correct.
    const FReferenceSkeleton& RefSkeleton = Skeleton->GetReferenceSkeleton();
    TArray<FBoneIndexType> RequiredBoneIndices;
    RequiredBoneIndices.SetNum(RefSkeleton.GetNum());
    for (int32 i = 0; i < RequiredBoneIndices.Num(); ++i)
    {
        RequiredBoneIndices[i] = i;
    }

    FBoneContainer BoneContainer;
    UE::Anim::FCurveFilterSettings CurveFilterSettings;
    BoneContainer.SetUseRAWData(true);
    BoneContainer.InitializeTo(RequiredBoneIndices, CurveFilterSettings, *Skeleton);

    // --- 3. Prepare Tracks ---
    // Pre-cache Socket Pointers to avoid finding them every frame
    struct FSocketCache
    {
        FName Name;
        USkeletalMeshSocket* SocketPtr;
        int32 TrackIndex;
    };
    TArray<FSocketCache> TargetSockets;

    for (const FName& Name : SocketNames)
    {
        if (USkeletalMeshSocket* Socket = Skeleton->FindSocket(Name))
        {
            FSocketCache Cache;
            Cache.Name = Name;
            Cache.SocketPtr = Socket;
            
            FBakedSocketTrack NewTrack;
            NewTrack.SocketName = Name;
            Cache.TrackIndex = NewBakedData->SocketTracks.Add(NewTrack);
            
            TargetSockets.Add(Cache);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("BakeSockets: Could not find socket '%s' on Skeleton."), *Name.ToString());
        }
    }

    if (TargetSockets.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("BakeSockets: None of the requested sockets exist on the skeleton."));
        return nullptr;
    }

    // --- 4. Iterate Frames and Bake ---
    const int32 NumFrames = SourceAnimation->GetNumberOfSampledKeys();
    const double SampleRate = SourceAnimation->GetSamplingFrameRate().AsDecimal();
    const double Interval = (SampleRate > 0.0) ? (1.0 / SampleRate) : 0.0333; // Default to 30fps if invalid

    FCSPose<FCompactPose> ComponentSpacePose;

    for (int32 FrameIndex = 0; FrameIndex < NumFrames; ++FrameIndex)
    {
        const double Time = FrameIndex * Interval;

        // Get the pose for this time
        GetComponentSpacePoseAtTime(SourceAnimation, Time, BoneContainer, ComponentSpacePose);

        // Process each requested socket
        for (const FSocketCache& SocketInfo : TargetSockets)
        {
            FTransform FinalTransform = FTransform::Identity;

            if (SocketInfo.SocketPtr)
            {
                // Find the index of the bone the socket is attached to
                int32 BoneIndex = Skeleton->GetReferenceSkeleton().FindBoneIndex(SocketInfo.SocketPtr->BoneName);
                
                if (BoneIndex != INDEX_NONE)
                {
                    FCompactPoseBoneIndex CompactIndex = BoneContainer.MakeCompactPoseIndex(FMeshPoseBoneIndex(BoneIndex));
                    
                    if (CompactIndex != INDEX_NONE)
                    {
                        // Get Bone Transform in Component Space
                        const FTransform& BoneTransform = ComponentSpacePose.GetComponentSpaceTransform(CompactIndex);
                        
                        // Apply Socket Relative Transform
                        // Socket World = SocketRelative * BoneLocal * ParentBoneLocal ...
                        // Since we have BoneCS, it is: SocketRelative * BoneCS
                        FinalTransform = SocketInfo.SocketPtr->GetSocketLocalTransform() * BoneTransform;
                    }
                }
            }

            // Store Data
            FBakedSocketTransform FrameData;
            FrameData.Location = FinalTransform.GetLocation();
            FrameData.Rotation = FinalTransform.GetRotation().Rotator();;
            FrameData.Scale = FinalTransform.GetScale3D();

            NewBakedData->SocketTracks[SocketInfo.TrackIndex].Frames.Add(FrameData);
        }
    }

    // --- 5. Finalize Asset ---
    NewBakedData->MarkPackageDirty();
    FAssetRegistryModule::AssetCreated(NewBakedData);
    
    UE_LOG(LogTemp, Log, TEXT("BakeSockets: Successfully baked %d sockets over %d frames to %s"), TargetSockets.Num(), NumFrames, *FinalAssetName);

    return NewBakedData;
#else
    UE_LOG(LogTemp, Error, TEXT("BakeSockets: This function is Editor Only."));
    return nullptr;
#endif
}