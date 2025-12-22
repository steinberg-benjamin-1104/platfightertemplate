#include "BakedSockets.h"

bool UBakedAnimation::GetSocketTransform(FName SocketName, int32 Frame, bool bMirrored, FTransform& OutTransform) const
{
	// 1. Locate the specific track for the requested socket
	// Using FindByPredicate is efficient for a small/medium number of sockets
	const FBakedSocketTrack* FoundTrack = SocketTracks.FindByPredicate([&](const FBakedSocketTrack& Track) {
		return Track.SocketName == SocketName;
	});

	// 2. Validate track existence and frame index
	if (FoundTrack && FoundTrack->Frames.IsValidIndex(Frame))
	{
		const FBakedSocketTransform& Data = FoundTrack->Frames[Frame];
        
		// 3. Reconstruct the FTransform from the baked components
		OutTransform.SetLocation(Data.Location);
		OutTransform.SetRotation(Data.Rotation.Quaternion()); // Convert Rotator back to Quat for math
		OutTransform.SetScale3D(Data.Scale);
        
		// 4. Handle Mirroring if requested
		// Note: EAxis::Y is standard for Unreal's lateral axis mirroring
		if (bMirrored)
		{
			OutTransform.Mirror(EAxis::Y, EAxis::Y); 
		}
        
		return true;
	}

	// 5. Fallback: If socket or frame is missing, return Identity and false
	OutTransform = FTransform::Identity;
	return false;
}