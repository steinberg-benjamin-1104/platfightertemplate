#include "BakeSocketsLibrary.h"
#include "Animation/AnimSequence.h"
#include "Animation/Skeleton.h"
#include "Engine/SkeletalMeshSocket.h"
#include "AssetToolsModule.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "BonePose.h"
#include "BakedSockets.h"
#include "BakeProfile.h"

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
    const UBakeProfile* Profile,
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

    const FReferenceSkeleton& RefSkeleton = Skeleton->GetReferenceSkeleton();

    // --- 1. Identify Target Names (From Profile or Skeleton) ---
    TArray<FName> NamesToBake;
    if (Profile && Profile->TargetNames.Num() > 0)
    {
        NamesToBake = Profile->TargetNames;
    }
    else
    {
        for (USkeletalMeshSocket* Socket : Skeleton->Sockets) { NamesToBake.Add(Socket->SocketName); }
        for (int32 i = 0; i < RefSkeleton.GetNum(); ++i) { NamesToBake.Add(RefSkeleton.GetBoneName(i)); }
    }

    // --- 2. Create the New Asset ---
    IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
    FString PackageName;
    FString FinalAssetName;
    AssetTools.CreateUniqueAssetName(AssetPath / AssetName, TEXT(""), PackageName, FinalAssetName);

    UPackage* Package = CreatePackage(*PackageName);
    UBakedAnimation* NewBakedData = NewObject<UBakedAnimation>(Package, UBakedAnimation::StaticClass(), *FinalAssetName, RF_Public | RF_Standalone);
    if (!NewBakedData) return nullptr;

    // Fixed naming: assuming SourceAnimation is the property name in UBakedAnimation
    NewBakedData->AnimSequence = SourceAnimation; 
    NewBakedData->SocketTracks.Empty();

    // --- 3. Initialize Animation Context ---
    TArray<FBoneIndexType> RequiredBoneIndices;
    RequiredBoneIndices.SetNum(RefSkeleton.GetNum());
    for (int32 i = 0; i < RequiredBoneIndices.Num(); ++i) { RequiredBoneIndices[i] = i; }

    FBoneContainer BoneContainer;
    BoneContainer.SetUseRAWData(true);
    UE::Anim::FCurveFilterSettings CurveFilterSettings;
    BoneContainer.InitializeTo(RequiredBoneIndices, CurveFilterSettings, *Skeleton);

    // --- 4. Prepare Tracks & Cache ---
    struct FBakeTarget {
        FName Name;
        int32 BoneIndex;
        USkeletalMeshSocket* SocketPtr;
        int32 TrackIndex;
    };
    TArray<FBakeTarget> BakeTargets;

    for (const FName& RequestedName : NamesToBake)
    {
        USkeletalMeshSocket* FoundSocket = Skeleton->FindSocket(RequestedName);
        FName BoneToFind = FoundSocket ? FoundSocket->BoneName : RequestedName;
        int32 FoundBoneIndex = RefSkeleton.FindBoneIndex(BoneToFind);

        if (FoundBoneIndex != INDEX_NONE)
        {
            FBakeTarget Target;
            Target.Name = RequestedName;
            Target.SocketPtr = FoundSocket;
            Target.BoneIndex = FoundBoneIndex;
            
            FBakedSocketTrack NewTrack;
            NewTrack.SocketName = RequestedName;
            Target.TrackIndex = NewBakedData->SocketTracks.Add(NewTrack);
            BakeTargets.Add(Target);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("BakeSockets: [%s] skipped. No bone/socket found on Skeleton [%s]."), 
                *RequestedName.ToString(), *Skeleton->GetName());
        }
    }

    if (BakeTargets.Num() == 0) return nullptr;

    // --- 5. Iterate Frames and Bake ---
    const int32 NumFrames = SourceAnimation->GetNumberOfSampledKeys();
    const double Duration = SourceAnimation->GetPlayLength();
    const double Interval = (NumFrames > 1) ? (Duration / (NumFrames - 1)) : 0.0f;

    for (int32 FrameIndex = 0; FrameIndex < NumFrames; ++FrameIndex)
    {
        FMemMark Mark(FMemStack::Get());
        const double Time = FrameIndex * Interval;

        FCompactPose Pose;
        Pose.SetBoneContainer(&BoneContainer);
        FBlendedCurve Curve;
        Curve.InitFrom(BoneContainer);
        UE::Anim::FStackAttributeContainer Attributes;
        FCSPose<FCompactPose> ComponentSpacePose;

        FAnimationPoseData AnimationPoseData(Pose, Curve, Attributes);
        FAnimExtractContext Context(Time, SourceAnimation->bLoop);
        SourceAnimation->GetBonePose(AnimationPoseData, Context);

        ComponentSpacePose.InitPose(Pose);

        // --- ROOT MOTION EXTRACTION LOGIC ---
        // Get the actual Root Bone (index 0) transform for this frame
        FCompactPoseBoneIndex RootBoneIndex(0);
        FTransform RootTransform = ComponentSpacePose.GetComponentSpaceTransform(RootBoneIndex);

        for (const FBakeTarget& Target : BakeTargets)
        {
            FCompactPoseBoneIndex CompactIndex = BoneContainer.MakeCompactPoseIndex(FMeshPoseBoneIndex(Target.BoneIndex));
            FTransform BoneTransform = ComponentSpacePose.GetComponentSpaceTransform(CompactIndex);

            if (Target.SocketPtr)
            {
                BoneTransform = Target.SocketPtr->GetSocketLocalTransform() * BoneTransform;
            }

            FTransform FinalTransform;

            // If this is the root bone, keep it in Component Space (the 'World' movement)
            // If it's anything else, make it relative to the Root Bone
            if (Target.BoneIndex == 0 && !Target.SocketPtr)
            {
                FinalTransform = BoneTransform;
            }
            else
            {
                // Make relative to root to prevent double-transform when moving Actor
                FinalTransform = BoneTransform.GetRelativeTransform(RootTransform);
            }

            FBakedSocketTransform FrameData;
            FrameData.Location = FinalTransform.GetLocation();
            FrameData.Rotation = FinalTransform.GetRotation().Rotator();
            FrameData.Scale = FinalTransform.GetScale3D();

            NewBakedData->SocketTracks[Target.TrackIndex].Frames.Add(FrameData);
        }
    }

    NewBakedData->MarkPackageDirty();
    FAssetRegistryModule::AssetCreated(NewBakedData);
    
    UE_LOG(LogTemp, Log, TEXT("BakeSockets: Finished baking %d targets over %d frames."), BakeTargets.Num(), NumFrames);

    return NewBakedData;
#else
    return nullptr;
#endif
}