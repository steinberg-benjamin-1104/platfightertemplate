#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "FighterStateMachine.h"
#include "HitboxManagerComponent.h"
#include "FighterPlayerController.h"
#include "Animation.h"
#include "FrameScriptRunner.h"
#include "ShieldComponent.h"
#include "Hittable.h"
#include "CamTarget.h"
#include "FighterMovementComponent.h"
#include "BoneVectorAxis.h"
#include "AttackDefinition.h"
#include "FighterAnimInstance.h"
#include "StickDirection.h"
#include "BakedSockets.h"
#include "InputBuffer.h"

#include "FighterPawn.generated.h"

class UFighterMovementComponent;
class UBoxComponent;
class USkeletalMeshComponent;
class UFighterStateMachine;
class UFighterAnimInstance;
class AHurtbox2D;
class UCharacterPanelWidget;

UCLASS()
class PFTEMPLATE_API AFighterPawn : public APawn, public IHittable, public ICamTarget
{
	GENERATED_BODY()

public:
	AFighterPawn();

	//simulated tick phases
	void PreCollisionPhase(int32 CurrentFrame);
	void UpdateAnimation();
	void DetectCollisions();
	void ProcessCollisions();
	void ShieldPhase();
	void PostCollisionPhase();
	void FighterDebug();
	
	UFUNCTION(BlueprintPure, Category = "Component Getters")
	UFighterMovementComponent* GetFighterMovementComponent() const {return MovementComponent;}

	UFUNCTION(BlueprintPure, Category = "Component Getters")
	UHitboxManagerComponent* GetHitboxManager() {return HitboxManager;}

	UFUNCTION(BlueprintPure, Category = "Component Getters")
	USkeletalMeshComponent* GetMesh() const {return CharacterMesh;}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Componenet")
	UFighterMovementComponent* MovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UFighterStateMachine* StateMachine;

	//StateMachine->AddState("Fireball", NewObject<UFireballState>(FSM));
	UFUNCTION(BlueprintCallable, Category = "FSM")
	virtual void RegisterCustomStates(UFighterStateMachine* FSM){}
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UFrameScriptRunnerComponent* FrameScriptRunner;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UShieldComponent* ShieldComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	FAnimation CurrentAnimation;

	UFUNCTION(BlueprintPure, Category = "Animation")
	FAnimation GetCurrentAnimation() const {return CurrentAnimation; }
	
	bool SetCurrentAnimation(FName AnimationName, int32 BlendTime = 0);
	bool SetCurrentAnimation(const FAnimation& NewAnimation, int32 BlendTime = 0);

	UPROPERTY() AFighterPlayerController* FPC = nullptr;

	UFUNCTION(BlueprintPure, Category = "Movement")
	bool IsFacingRight() const;

	UFUNCTION(BlueprintPure, Category = "Movement")
	int32 GetFacingDirection() const {return IsFacingRight() ? 1 : -1;}

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void SetFacingDirection(int32 Direction);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void FlipFacingDirection();

	UFUNCTION(BlueprintCallable, Category = "Hurtbox")
	void ToggleAllHurtboxesInvulnerable(bool bEnable);

	void ToggleHurtboxInvulnerable(FName HurtboxSuffix, bool bEnable);

	void InitHurtboxes();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animations")
	UDataTable* AnimationTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attacks")
	UDataTable* AttackTable;
	
	FAnimation GetAnimationByName(FName MoveName) const;
	const FAttackDefinition* DetermineAttack(EInputButton InputButton, bool bFlickInput, EStickDir StickDir) const;

	bool TryStartAttack(EInputButton Button);

	bool bStopAnimUpdates = false;
	bool bStopMvmtUpdates = false;

	void FreezePlayer(bool bFreeze);

	// 0 - 9999
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	int32 Health = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	int32 StocksLeft = 3;

	UFUNCTION(BlueprintPure, Category = "Damage")
	float GetHealthFloat() const {return Health/10.f;}

	UFUNCTION(BlueprintCallable, Category = "Damage")
	void ApplyDamage(int32 Damage);

	UFUNCTION(BlueprintCallable, Category = "Damage")
	void FaceInstigator();

	UFUNCTION(BlueprintCallable, Category = "Damage")
	void SetStocks(int32 StockCount);

	UFUNCTION(BlueprintCallable, Category = "Damage")
	void LoseStock();

	UFUNCTION(BlueprintCallable, Category = "Damage")
	void ResetHealth() {Health = 0;}

	virtual void WasHit(const FDamageInfo& DamageInfo, AFighterPawn* Instigator) override;

	virtual FVector CenterLocation() override {return Fixed2DToVector(MovementComponent->GetCenter());}
	
	float GetBakedBoneRotation(FName BoneName);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	EBoneVectorAxis BoneVectorDirection = EBoneVectorAxis::Forward;

	UFUNCTION(BlueprintCallable, Category = "Mesh")
	void ShakeMesh();

	UFUNCTION(BlueprintCallable, Category = "Mesh")
	void ResetMeshOffset();
	
	void InitiateKnockback();
	void HandleParry();

	UPROPERTY() FDamageInfo StoredDamageInfo;
	UPROPERTY() AFighterPawn* LastInstigator;

	UPROPERTY(BlueprintReadWrite, Category = "CharStats")
	int32 Weight = 100;

	void SetCharacterPanel(UCharacterPanelWidget* Panel) { CharacterPanel = Panel; }

	bool bParry = false;

	FFixedVector2D GetFixedLoc() const { return VectorToFixed2D(GetActorLocation()); }
	void SetFixedLoc(FFixedVector2D InLoc) { SetActorLocation(Fixed2DToVector(InLoc)); }

	bool AnimFinished() {return FighterAnimInstance->CurrentAnimFinished(); }

	FBakedSocketKey GetBakedSocketKey(FName SocketName);
	FFixedVector2D GetBakedSocketLocation(FName SocketName);

	UPROPERTY() FInputBuffer InputBuffer;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	UHitboxManagerComponent* HitboxManager;

	UPROPERTY() UFighterAnimInstance* FighterAnimInstance;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Hurtboxes")
	TMap<FName, AHurtbox2D*> HurtboxMap;
	
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

	UPROPERTY(VisibleAnywhere, Category = "Shield|Visual")
	UStaticMeshComponent* ShieldMesh;

private:

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* CharacterMesh;

	UPROPERTY() UCharacterPanelWidget* CharacterPanel;
	
	int32 ShakeSign = 1;
	FVector RootPreviousFrame = FVector::ZeroVector;
};