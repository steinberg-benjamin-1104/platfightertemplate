#include "BakedSockets.h"

FTransform UBakedAnimation::GetSocketTransform(FName SocketName, int32 Frame, bool bMirrored) const
{
	const FBakedSocketTrack* FoundTrack = SocketTracks.FindByPredicate([&](const FBakedSocketTrack& Track) {
		return Track.SocketName == SocketName;
	});
	
	if (FoundTrack && FoundTrack->Frames.IsValidIndex(Frame))
	{
		const FBakedSocketTransform& Data = FoundTrack->Frames[Frame];
		FTransform OutTransform;
		
		OutTransform.SetLocation(Data.Location);
		OutTransform.SetRotation(Data.Rotation.Quaternion());
		OutTransform.SetScale3D(Data.Scale);
        
		
		if (bMirrored) OutTransform.Mirror(EAxis::Y, EAxis::Y); 
		return OutTransform;
	}
	return FTransform::Identity;
}