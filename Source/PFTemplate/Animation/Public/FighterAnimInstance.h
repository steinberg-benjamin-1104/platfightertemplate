#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AnimCursor.h"
#include "SafeMath.h"
#include "FighterAnimInstance.generated.h"

USTRUCT()
struct FBlendProfile
{
	GENERATED_BODY()

	FBlendProfile(int32 TotFrames) { TotalFrames = TotFrames; }
	
	int32 Frame = 0;
	int32 TotalFrames = 0;
	float Alpha = 0.f;

	bool CanBlend() { return Frame < TotalFrames; }
	void Advance()
	{
		if (CanBlend)
		{
			Frame++;
			FFixed_32 TempAlpha = FixedAlphaFromFrame(Frame, TotalFrames);
			Alpha = FMath::Clamp(FixedToFloat(TempAlpha), 0.f, 1.f);
		}
		else Alpha = 1.f;
	}
};

USTRUCT()
struct FAnimSnapshot
{
	GENERATED_BODY()

	bool bStopAnimUpdates = false;
	
	FAnimCursor Current;
	FAnimCursor Previous;

	FBlendProfile Blend;

	void SetNewAnim(FAnimCursor NewAnim, FBlendProfile NewProf)
	{
		Previous = Current;
		Current = NewAnim;
		Blend = NewProf;
	}
};

UCLASS()
class PFTEMPLATE_API UFighterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

	FAnimSnapshot AnimSnapshot;
	
public:
	
	void SetAnimationSequence(UBakedAnimation* NewAnim, bool bLoop = false, int32 InBlendFrames = 0);
	void AdvanceFrame();
	UAnimSequence* GetCurrentAnim() { return AnimSnapshot.Current.Animation->AnimSequence; }
	UAnimSequence* GetPreviousAnim() { return AnimSnapshot.Previous.Animation->AnimSequence; }
	int32 GetFrame() { return AnimSnapshot.Current.Frame; }
	float GetAlpha() { return AnimSnapshot.Blend.Alpha; }
	
};