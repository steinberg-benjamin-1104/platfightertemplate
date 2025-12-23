#include "BakedSockets.h"

FBakedSocketKey UBakedAnimation::GetSocketKey(FName SocketName, int32 Frame, bool bMirrored) const
{
	const FBakedSocketTrack* FoundTrack = SocketTracks.FindByPredicate([&](const FBakedSocketTrack& Track) {
	   return Track.SocketName == SocketName;
	});
    
	if (FoundTrack && FoundTrack->Frames.IsValidIndex(Frame))
	{
		FBakedSocketKey Data = FoundTrack->Frames[Frame];
       
		if (bMirrored) 
		{
			Data.Location2D.X = -Data.Location2D.X;
			Data.RotationAngle = 180.0f - Data.RotationAngle;
		} 
		return Data;
	}
    
	return FBakedSocketKey();
}