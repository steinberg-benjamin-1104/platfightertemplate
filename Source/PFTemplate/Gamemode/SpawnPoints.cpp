#include "SpawnPoints.h"
#include "Components/BillboardComponent.h"
#include "Components/ArrowComponent.h"

ASpawnPoints::ASpawnPoints()
{
	PrimaryActorTick.bCanEverTick = false;
	
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	// Billboard
	Billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	Billboard->SetupAttachment(RootComponent);
	Billboard->SetRelativeLocation(FVector(0.f, 0.f, height));
	Billboard->bHiddenInGame = true;

	// Arrow
	FacingArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("FacingArrow"));
	FacingArrow->SetupAttachment(RootComponent);
	FacingArrow->SetRelativeLocation(FVector(0.f, 0.f, height));
	FacingArrow->ArrowSize = 1.5f;
	FacingArrow->bHiddenInGame = true;
}

#if WITH_EDITOR
void ASpawnPoints::OnConstruction(const FTransform& Transform)
{
	// Flip arrow direction based on FacingRight
	if (bFacingRight)
	{
		FacingArrow->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));   // Point right (X+)
	}
	else
	{
		FacingArrow->SetRelativeRotation(FRotator(0.f, 180.f, 0.f)); // Point left (X-)
	}
}
#endif