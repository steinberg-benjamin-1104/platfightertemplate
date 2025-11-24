#include "StickState.h"
#include "ButtonState.h"

void UpdateStickState(FStickState& S, FFixedVector2D NewStickInput)
{
	S.PrepareForNextFrame(NewStickInput);

	S.bDownThisFrame  = (S.Current.Z < -S.DownThreshold) && !(S.Previous.Z < -S.DownThreshold);
	S.bUpThisFrame    = (S.Current.Z >  S.DownThreshold) && !(S.Previous.Z >  S.DownThreshold);

	S.bLeftThisFrame  = (S.Current.X < -S.DownThreshold) && !(S.Previous.X < -S.DownThreshold);
	S.bRightThisFrame = (S.Current.X >  S.DownThreshold) && !(S.Previous.X >  S.DownThreshold);

	bool FlickX = (S.Current.X.Abs() > S.FlickThreshold && S.Previous.X.Abs() < S.FlickThreshold);
	bool FlickY = (S.Current.Z.Abs() > S.FlickThreshold && S.Previous.Z.Abs() < S.FlickThreshold);

	S.bFlick = FlickX || FlickY;
	S.FlickDirection = S.bFlick ? S.Current : FFixedVector2D();

	S.bWalking = (S.Current.X.Abs() <= S.WalkRadius);
}