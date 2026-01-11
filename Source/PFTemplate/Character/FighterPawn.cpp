#include "FighterPawn.h"
#include "FighterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "FighterAnimInstance.h"
#include "Hurtbox2D.h"
#include "FrameScriptRunner.h"
#include "ShieldComponent.h"
#include "CharacterPanelWidget.h"
#include "AttackDefinition.h"
#include "PFTemplate/Combat/Projectiles/ProjectilePool.h"

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

	EffectMachine = NewObject<UEffectMachine>(this);
	EffectMachine->Initialize(this);
	
	HitboxManager->Initialize(this, 16);
	FrameScriptRunner->Initialize(this);

	ShieldComponent->InitShield(ShieldMesh);

	for (UProjectilePool* pool : ProjectilePools) pool->Initialize(this);
	
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

void AFighterPawn::PreCollisionPhase(int32 CurrentFrame)
{
	FFighterInput NewInput;
	if (FPC) FPC->UpdateInput(CurrentFrame, NewInput);
	InputBuffer.Update(NewInput);
	if (EffectMachine) EffectMachine->UpdateEffectTimers();
	if (StateMachine) StateMachine->TickCurrentState();
	if (MovementComponent && !bStopMvmtUpdates) MovementComponent->TickFMC();
	if (CharacterMesh && FighterAnimInstance && !bStopAnimUpdates) UpdateAnimation();
	for (UProjectilePool* pool : ProjectilePools) pool->PreCollision();
}

void AFighterPawn::UpdateAnimation()
{
	FighterAnimInstance->AdvanceFrame();
	FighterAnimInstance->UpdateAnimation(FixedToFloat(FixedDt), false);
	CharacterMesh->RefreshBoneTransforms();
	CharacterMesh->FinalizeBoneTransform();
	CharacterMesh->UpdateComponentToWorld();

	MovementComponent->ApplyAnimMovement(FighterAnimInstance->GetCurrentFrameIndex());
	
	for (auto& Pair : HurtboxMap)
	{
		if (AHurtbox2D* Hurtbox = Pair.Value) Hurtbox->TickHurtbox();
	}
	
	FrameScriptRunner->TickScript(FighterAnimInstance->GetCurrentFrameIndex());
}

void AFighterPawn::DetectCollisions()
{
	if (HitboxManager) HitboxManager->ScanActiveHitboxes();
	for (UProjectilePool* pool : ProjectilePools) pool->DetectCollision();
}

void AFighterPawn::ProcessCollisions()
{
	if (HitboxManager) HitboxManager->ProcessHits();
	for (UProjectilePool* pool : ProjectilePools) pool->ProcessHits();
}

void AFighterPawn::PostCollisionPhase()
{
	ShieldPhase();
	if (EffectMachine) EffectMachine->UpdateActiveEffects();
}

void AFighterPawn::ShieldPhase()
{
	ShieldComponent->UpdateShield();
	if (ShieldComponent->IsBroken())
	{
		StateMachine->ChangeFighterState("Shieldbreak");
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
	SetFacingDirection(LastInstigator->GetFixedLoc().X < GetFixedLoc().X ? -1 : 1 );
}

#pragma endregion

#pragma region Animation

FAnimation AFighterPawn::GetAnimationByName(FName MoveName) const
{
	if (!AnimationTable) return FAnimation();

	static const FString Context = TEXT("Move Lookup");
	const FAnimation* Row = AnimationTable->FindRow<FAnimation>(MoveName, Context, true);

	if (Row) return *Row;
	
	return FAnimation();
}

bool AFighterPawn::SetCurrentAnimation(FName AniName, int32 BlendTime)
{
	const FAnimation NewAni = GetAnimationByName(AniName);

	if (!NewAni.BakedAnimation) return false;

	UAnimSequence* AnimSequence = NewAni.BakedAnimation->AnimSequence;

	const int32 NumFrames = AnimSequence->GetNumberOfSampledKeys();
	CurrentAnimation = NewAni;

	if (UMoveLogicAsset* CommandAsset = NewAni.CommandScript)
		FrameScriptRunner->LoadScript(CommandAsset->Commands);
	else
		FrameScriptRunner->LoadScript({});
	
	FighterAnimInstance->SetAnimationSequence(AnimSequence, NewAni.bIsLoop, NumFrames, BlendTime);
	MovementComponent->SetBakedMovement(NewAni.BakedAnimMvmt);
	return true;
}

bool AFighterPawn::SetCurrentAnimation(const FAnimation& NewAni, int32 BlendTime)
{
	if (!NewAni.BakedAnimation) return false;

	UAnimSequence* AnimSequence = NewAni.BakedAnimation->AnimSequence;
	if (!AnimSequence) return false;
	
	const int32 NumFrames = AnimSequence->GetNumberOfSampledKeys();
	CurrentAnimation = NewAni;

	if (UMoveLogicAsset* CommandAsset = NewAni.CommandScript)
		FrameScriptRunner->LoadScript(CommandAsset->Commands);
	else
		FrameScriptRunner->LoadScript({});
	
	FighterAnimInstance->SetAnimationSequence(AnimSequence, NewAni.bIsLoop, NumFrames, BlendTime);
	MovementComponent->SetBakedMovement(NewAni.BakedAnimMvmt);

	return true;
}

const FAttackDefinition* AFighterPawn::DetermineAttack(EInputButton InputMask, bool bFlickInput, EStickDir StickDir) const
{
	if (!AttackTable) return nullptr;

	const FName CurrentState = StateMachine->CurrentStateKey;

	TArray<FAttackDefinition*> Rows;
	AttackTable->GetAllRows<FAttackDefinition>(TEXT("AttackLookup"), Rows);

	for (const FAttackDefinition* Def : Rows)
	{
		if (Def->ValidStates.Num() > 0 && !Def->ValidStates.Contains(CurrentState))
			continue;

		uint16 DefMask = CompileMoveButtonMask(Def->MoveButton);

		// Check if all buttons in the attack definition are pressed
		if (DefMask != 0 && (static_cast<uint16>(InputMask) & DefMask) != DefMask)
			continue;

		if (Def->bFlickInput != bFlickInput)
			continue;

		if (Def->StickDir != EStickDir::Any & Def->StickDir != StickDir)
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
		case EHitEffect::Grab:
			HandleGrab();
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
	StateMachine->ChangeFighterState("Hitstop");
}

void AFighterPawn::HandleGrab()
{
	StateMachine->ChangeFighterState("GrabHeld");
	LastInstigator->StateMachine->ChangeFighterState("GrabHold");
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

bool AFighterPawn::TryStartAttack(EInputButton Button, FFighterInput* Input)
{
	const EStickDir StickDir = GetStickDirection(Input->StickPos, IsFacingRight());
	
	if (const FAttackDefinition* Attack = DetermineAttack(Button, Input->IsPressed(EInputButton::Flick), StickDir))
	{
		if (SetCurrentAnimation(Attack->Animation))
		{
			StateMachine->ChangeFighterState(Attack->TargetState);
			return true;
		}
	}
	return false;
}

FBakedSocketKey AFighterPawn::GetBakedSocketKey(FName SocketName)
{
	return CurrentAnimation.BakedAnimation->GetSocketKey(
		SocketName,
		FighterAnimInstance->GetCurrentFrameIndex(),
		!IsFacingRight());
}

FFixedVector2D AFighterPawn::GetBakedSocketLocation(FName SocketName)
{
	FVector2D Loc = GetBakedSocketKey(SocketName).Location2D;
	return Vector2DToFixed2D(Loc) + GetFixedLoc();
}

float AFighterPawn::GetBakedBoneRotation(FName SocketName)
{
	return GetBakedSocketKey(SocketName).RotationAngle;
}