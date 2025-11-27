#include "FighterPawn.h"
#include "FighterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "FighterAnimInstance.h"
#include "Hurtbox2D.h"
#include "FrameScriptRunner.h"
#include "ShieldComponent.h"
#include "CharacterPanelWidget.h"
#include "ButtonState.h"
#include "AttackDefinition.h"

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

	MovementComponent->InitFMC(this);
	
	StateMachine = NewObject<UFighterStateMachine>(this);
	StateMachine->Initialize(this);
	
	HitboxManager->Initialize(this, 16);
	FrameScriptRunner->Initialize(this);

	ShieldComponent->InitShield(ShieldMesh);
	
	InitHurtboxes();
	SetCurrentAnimation("Idle");
}

void AFighterPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	FPC = Cast<AFighterPlayerController>(NewController);
}

#pragma endregion

#pragma region BattleManager

void AFighterPawn::InputPhase(int CurrentFrame)
{
	FFighterInput NewInput;
	if (FPC) FPC->UpdateInput(CurrentFrame, NewInput);
	if (StateMachine) StateMachine->TickCurrentState(NewInput);
}

void AFighterPawn::UpdateState()
{
	
}

void AFighterPawn::UpdateAnimation()
{
	if (MovementComponent || !bStopMvmtUpdates) MovementComponent->TickFMC();
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
	FFighterInput None;
	if (ShieldComponent->IsBroken())
	{
		StateMachine->TryChangeState("Shieldbreak", None);
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

void AFighterPawn::SetFacingDirection(int32 inDirection)
{
	float DesiredYaw = inDirection == 1 ? -90.f : 90.f;
	if (CharacterMesh)
	{
		CharacterMesh->SetRelativeRotation(FRotator(0.f, DesiredYaw, 0.f));
	}
}

void AFighterPawn::FlipFacingDirection()
{
	SetFacingDirection(IsFacingRight() ? -1 : 1);
}

void AFighterPawn::FaceInstigator()
{
	SetFacingDirection(LastInstigator->GetActorLocation().X < GetActorLocation().X ? -1.f : 1.f);
}

#pragma endregion

#pragma region Actions

FAnimation AFighterPawn::GetAnimationByName(FName MoveName) const
{
	if (!AnimationTable) return FAnimation();

	static const FString Context = TEXT("Move Lookup");
	const FAnimation* Row = AnimationTable->FindRow<FAnimation>(MoveName, Context, true);

	if (Row)
	{
		return *Row;
	}
	return FAnimation(); // failsafe
}

bool AFighterPawn::SetCurrentAnimation(FName AniName, int BlendTime)
{
	const FAnimation NewAni = GetAnimationByName(AniName);

	if (!NewAni.AnimSequence || !FrameScriptRunner || !FighterAnimInstance) return false;

	const int32 NumFrames = NewAni.AnimSequence->GetNumberOfSampledKeys();
	CurrentAnimation = NewAni;

	FrameScriptRunner->LoadScript(NewAni, NumFrames);

	FighterAnimInstance->SetAnimationSequence(NewAni.AnimSequence, NewAni.bIsLoop, NumFrames, BlendTime);

	return true;
}

bool AFighterPawn::SetCurrentAnimation(const FAnimation* NewAni, int BlendTime)
{
	if (!NewAni.AnimSequence || !FrameScriptRunner || !FighterAnimInstance) return false;

	const int32 NumFrames = NewAni.AnimSequence->GetNumberOfSampledKeys();
	CurrentAnimation = NewAni;

	FrameScriptRunner->LoadScript(NewAni, NumFrames);

	FighterAnimInstance->SetAnimationSequence(NewAni.AnimSequence, NewAni.bIsLoop, NumFrames, BlendTime);

	return true;
}

const FAttackDefinition* AFighterPawn::DetermineAttack(EInputButton InputButton, bool bFlickInput, EStickDir StickDir) const
{
	if (!AttackTable)
		return nullptr;

	const FName CurrentState = StateMachine->CurrentStateKey;

	TArray<FAttackDefinition*> Rows;
	AttackTable->GetAllRows<FAttackDefinition>(TEXT("AttackLookup"), Rows);

	for (const FAttackDefinition* Def : Rows)
	{
		if (Def->ValidStates.Num() > 0 && !Def->ValidStates.Contains(CurrentState))
			continue;
		
		if (Def->InputButton != EInputButton::None &&
			Def->InputButton != InputButton)
			continue;
		
		if (Def->bFlickInput != bFlickInput)
			continue;
		
		if (Def->StickDir != EStickDir::Center &&
			Def->StickDir != StickDir)
			continue;
		
		return Def;
	}

	return nullptr;
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
	FFighterInput None;
	ApplyDamage(StoredDamageInfo.Damage);
	StateMachine->TryChangeState("Hitstop", None);
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

bool AFighterPawn::TryStartAttack(EInputButton Button, FFighterInput& Input)
{
	const FStickState& StickState = Input.Stick;
	
	const FFixedVector2D StickPos = StickState.bFlick ? StickState.FlickPosition : StickState.Current;
	const EStickDir StickDir = GetStickDirection(StickPos, IsFacingRight());
	
	const FAttackDefinition* Attack = DetermineAttack(Button, StickState.bFlick, StickDir);
	if (!Attack) return false;
	
	const FAnimation* Anim = Attack->AnimationRow.GetRow<FAnimation>(TEXT("AttackAnimLookup"));
	SetCurrentAnimation(Anim);
	
	StateMachine->TryChangeState(Attack->TargetState, Input);
	
	return true;
}
