#include "Stage.h"
#include "DrawDebugHelpers.h"

AStage::AStage()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AStage::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bDrawDebug) return;

	UWorld* World = GetWorld();
	if (!World) return;
	
	DrawBlastzone(World);
}

void AStage::DrawBlastzone(UWorld* World)
{
	const FTransform& Xform = GetActorTransform();
	const FVector Origin = Xform.GetLocation();

	FVector BottomLeft  = Origin + FVector(Blastzone.Left,  0.f, Blastzone.Bottom);
	FVector BottomRight = Origin + FVector(Blastzone.Right, 0.f, Blastzone.Bottom);
	FVector TopLeft     = Origin + FVector(Blastzone.Left,  0.f, Blastzone.Top);
	FVector TopRight    = Origin + FVector(Blastzone.Right, 0.f, Blastzone.Top);

	const FColor Color = FColor::Red;
	const float thickness = 5.f;

	DrawDebugLine(World, BottomLeft,  BottomRight, Color, false, -1.f, 0, thickness);
	DrawDebugLine(World, BottomRight, TopRight,    Color, false, -1.f, 0, thickness);
	DrawDebugLine(World, TopRight,    TopLeft,     Color, false, -1.f, 0, thickness);
	DrawDebugLine(World, TopLeft,     BottomLeft,  Color, false, -1.f, 0, thickness);
}