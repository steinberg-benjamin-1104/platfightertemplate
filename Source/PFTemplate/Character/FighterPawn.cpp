#include "FighterPawn.h"
#include "FighterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "FighterAnimInstance.h"
#include "Hurtbox2D.h"
#include "FrameScriptRunner.h"
#include "ShieldComponent.h"
#include "CharacterPanelWidget.h"

//GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Green, TEXT("Hitstop Called"));

#pragma region FighterPawn
AFighterPawn::AFighterPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;
	
	CharacterMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh"));
	CharacterMesh->SetupAttachment(Root);
	CharacterMesh->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	CharacterMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CharacterMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	CharacterMesh->bPauseAnims = true;
	
	// Set movement driver to RootComponent
	MovementComponent = CreateDefaultSubobject<UFighterMovementComponent>(TEXT("FighterMovement"));
	MovementComponent->UpdatedComponent = RootComponent;
	MovementComponent->PrimaryComponentTick.bCanEverTick = false;
	MovementComponent->PrimaryComponentTick.bStartWithTickEnabled = false;

	//Component Setup
	FrameScriptRunner = CreateDefaultSubobject<UFrameScriptRunnerComponent>(TEXT("FrameScriptRunner"));
	HitboxManager = CreateDefaultSubobject<UHitboxManagerComponent>(TEXT("HitboxManager"));
	ShieldComponent = CreateDefaultSubobject<UShieldComponent>(TEXT("Shield"));
	ShieldComponent->SetupAttachment(Root);

	//Shield
	ShieldMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShieldMesh"));
	ShieldMesh->SetupAttachment(Root);
	ShieldMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ShieldMesh->SetVisibility(false);
}

void AFighterPawn::BeginPlay()
{
	Super::BeginPlay();
	
	FighterAnimInstance = Cast<UFighterAnimInstance>(CharacterMesh->GetAnimInstance());
	
	StateMachine = NewObject<UFighterStateMachine>(this);
	StateMachine->Initialize(this);
	
	HitboxManager->Initialize(this, 16);
	FrameScriptRunner->Initialize(this);

	ShieldComponent->InitShield(ShieldMesh);
	
	InitHurtboxes();
	SetCurrentAction("Idle");
}

void AFighterPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	FPC = Cast<AFighterPlayerController>(NewController);
}

#pragma endregion

#pragma region BattleManager

void AFighterPawn::UpdateStick()
{
	if (FPC) FPC->UpdateStick();
}

void AFighterPawn::UpdateState()
{
	if (StateMachine) StateMachine->TickCurrentState();
}

void AFighterPawn::InputPhase()
{
	if (FPC) FPC->UpdateInput();
	if (MovementComponent || !bStopMvmtUpdates) MovementComponent->TickComponent(0.f, ELevelTick::LEVELTICK_All, nullptr);
}

void AFighterPawn::UpdateAnimation()
{
	if (!CharacterMesh || !FighterAnimInstance || bStopAnimUpdates) return;

	FighterAnimInstance->AdvanceFrame();
	
	CharacterMesh->TickPose(1.f/60.f, true);
	CharacterMesh->RefreshBoneTransforms();
	CharacterMesh->FinalizeBoneTransform();
	CharacterMesh->ConditionalUpdateComponentToWorld();
	CharacterMesh->UpdateChildTransforms(EUpdateTransformFlags::None, ETeleportType::None);
	
	for (auto& Pair : HurtboxMap)
	{
		if (AHurtbox2D* Hurtbox = Pair.Value)
		{
			Hurtbox->TickHurtbox();
		}
	}
	
	FrameScriptRunner->TickScript();
}

void AFighterPawn::CollisionPhase()
{
	if (HitboxManager) HitboxManager->ScanActiveHitboxes();
}

void AFighterPawn::ProcessCollisions()
{
	if (HitboxManager) HitboxManager->ProcessHits();
}

void AFighterPawn::ShieldPhase()
{
	ShieldComponent->UpdateShield();
	if (ShieldComponent->IsBroken())
	{
		StateMachine->TryChangeState("Shieldbreak");
	}
}

void AFighterPawn::FighterDebug()
{
	StateMachine->ShowStateDebug();
	MovementComponent->DisplayDebug();
}

#pragma endregion

#pragma region FacingDirection

bool AFighterPawn::IsFacingRight() const
{
	if (!CharacterMesh) return true;

	float Yaw = CharacterMesh->GetRelativeRotation().Yaw;
	
	return (FMath::IsNearlyEqual(Yaw, -90.f, 1.0f));
}

void AFighterPawn::SetFacingDirection(float Direction)
{
	float DesiredYaw = (Direction >= 0.f) ? -90.f : 90.f;
	if (CharacterMesh)
	{
		CharacterMesh->SetRelativeRotation(FRotator(0.f, DesiredYaw, 0.f));
	}
}

void AFighterPawn::SwitchFacingDirection()
{
	if (!CharacterMesh) return;

	// Flip the current facing direction
	float NewDirection = IsFacingRight() ? -1.f : 1.f;
	SetFacingDirection(NewDirection);
}

void AFighterPawn::FaceInstigator()
{
	SetFacingDirection(LastInstigator->GetActorLocation().X < GetActorLocation().X ? -1.f : 1.f);
}

#pragma endregion

#pragma region Actions

FAction AFighterPawn::GetActionByName(FName MoveName) const
{
	if (!ActionTable) return FAction();

	static const FString Context = TEXT("Move Lookup");
	const FAction* Row = ActionTable->FindRow<FAction>(MoveName, Context, true);

	if (Row)
	{
		return *Row;
	}
	return FAction(); // failsafe
}

bool AFighterPawn::SetCurrentAction(FName ActionName, int BlendTime)
{
	const FAction NewAction = GetActionByName(ActionName);

	if (!NewAction.AnimSequence || !FrameScriptRunner || !FighterAnimInstance) return false;

	const int32 NumFrames = NewAction.AnimSequence->GetNumberOfSampledKeys();
	CurrentAction = NewAction;

	FrameScriptRunner->LoadScript(NewAction, NumFrames);

	FighterAnimInstance->SetAnimationSequence(NewAction.AnimSequence, NewAction.bIsLoop, NumFrames, BlendTime);
	
	if (!NewAction.TargetState.IsNone())
	{
		StateMachine->TryChangeState(NewAction.TargetState);
	}

	return true;
}

#pragma endregion

#pragma region Combat

void AFighterPawn::WasHit(const FDamageInfo& DamageInfo, AFighterPawn* Instigator)
{
	StoredDamageInfo = DamageInfo;
	LastInstigator = Instigator;
	if (bParry)
	{
		HandleParry();
		return;
	}
	
	switch (DamageInfo.HitEffect)
	{
		case EHitEffect::Knockback:
			InitiateKnockback();
			break;
		default:
			break;
	}
}

void AFighterPawn::InitiateKnockback()
{
	if (ShieldComponent->IsActive())
	{
		ShieldComponent->ApplyDamage(StoredDamageInfo.Damage);
		return;
	}
	ApplyDamage(StoredDamageInfo.Damage);
	StateMachine->TryChangeState("Hitstop");
}

void AFighterPawn::ApplyDamage(int32 Damage)
{
	Health += Damage;
	CharacterPanel->SetDamage(GetHealthFloat());
}

void AFighterPawn::SetStocks(int32 StockCount)
{
	StocksLeft = StockCount;
	CharacterPanel->SetStocks(StocksLeft);
}

void AFighterPawn::LoseStock()
{
	StocksLeft--;
	SetStocks(StocksLeft);
}

void AFighterPawn::HandleParry()
{
	
}


#pragma endregion

#pragma region Mesh

FVector AFighterPawn::GetBoneLocation(FName BoneName) const
{
	if (!CharacterMesh) return FVector::ZeroVector;

	const FVector Loc = CharacterMesh->GetSocketLocation(BoneName);
	return FVector(Loc.X, 0.0f, Loc.Z) - CharacterMesh->GetRelativeLocation();
}

FVector AFighterPawn::GetBoneVector(FName BoneName) const
{
	if (!CharacterMesh) return FVector::ZeroVector;

	const FRotator BoneRotation = CharacterMesh->GetSocketRotation(BoneName);
	FVector BaseVec = FVector::ZeroVector;

	switch (BoneVectorDirection)
	{
		case EBoneVectorAxis::Forward: BaseVec = FVector::ForwardVector; break;
		case EBoneVectorAxis::Right: BaseVec = FVector::RightVector;   break;
		case EBoneVectorAxis::Up: BaseVec = FVector::UpVector;      break;
		default: break;
	}
	
	return BoneRotation.RotateVector(BaseVec);
}

void AFighterPawn::ShakeMesh()
{
	const FVector Loc = CharacterMesh->GetRelativeLocation();
	const FVector ShakePos = {10.f * ShakeSign, 0.f, Loc.Z};
	ShakeSign = ShakeSign * -1.f;
	CharacterMesh->SetRelativeLocation(ShakePos);
}

void AFighterPawn::ResetMeshOffset()
{
	CharacterMesh->SetRelativeLocation(FVector::ZeroVector);
}

#pragma endregion

#pragma region Hurtboxes

void AFighterPawn::InitHurtboxes()
{
	HurtboxMap.Empty();
	
	TArray<AActor*> AttachedActors;
	GetAttachedActors(AttachedActors, true); // true = recursively include nested

	for (AActor* Actor : AttachedActors)
	{
		if (AHurtbox2D* Hurtbox = Cast<AHurtbox2D>(Actor))
		{
			Hurtbox->Initialize(this);
			HurtboxMap.Add(Hurtbox->BoneName, Hurtbox);
		}
	}
}

void AFighterPawn::ToggleAllHurtboxesInvulnerable(bool bActive)
{
	for (auto& Pair : HurtboxMap)
	{
		if (AHurtbox2D* Hurtbox = Pair.Value)
		{
			Hurtbox->SetInvincibility(bActive);
		}
	}
}

void AFighterPawn::ToggleHurtboxInvulnerable(FName HurtboxSuffix, bool bEnable)
{
	if (AHurtbox2D** FoundHurtbox = HurtboxMap.Find(HurtboxSuffix))
	{
		if (AHurtbox2D* Hurtbox = *FoundHurtbox)
		{
			Hurtbox->SetInvincibility(bEnable);
		}
	}
}


#pragma endregion

void AFighterPawn::FreezePlayer(bool bFreeze)
{
	bStopAnimUpdates = bFreeze;
	bStopMvmtUpdates = bFreeze;
}