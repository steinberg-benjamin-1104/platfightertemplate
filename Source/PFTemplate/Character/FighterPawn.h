#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "FighterStateMachine.h"
#include "EffectMachine.h"
#include "HitboxManagerComponent.h"
#include "FighterPlayerController.h"
#include "AnimationSlots.h"
#include "ShieldComponent.h"
#include "Hittable.h"
#include "CamTarget.h"
#include "FighterMovementComponent.h"
#include "AttackDefinition.h"
#include "FighterAnimInstance.h"
#include "StickDirection.h"
#include "BakedSockets.h"
#include "ProjectilePool.h"
#include "InputBuffer.h"

#include "FighterPawn.generated.h"

class UFighterMovementComponent;
class UBoxComponent;
class USkeletalMeshComponent;
class UFighterStateMachine;
class UEffectMachine;
class UFighterAnimInstance;
class AHurtbox2D;
class UCharacterPanelWidget;
class UProjectilePool;
class UCharacterAnimationSet;

UCLASS()
class PFTEMPLATE_API AFighterPawn : public APawn, public IHittable, public ICamTarget
{
	GENERATED_BODY()

public:

	int32 PlayerID;
	
	AFighterPawn();
	
	void PreCollisionPhase(int32 CurrentFrame);
	void UpdateDependencies();
	void UpdateAnimation();
	void DetectCollisions();
	void ProcessCollisions();
	void ShieldPhase();
	void PostCollisionPhase();
	void FighterDebug();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Componenet")
	UFighterMovementComponent* MovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UFighterStateMachine* StateMachine;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	UHitboxManagerComponent* HitboxManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UEffectMachine* EffectMachine;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UShieldComponent* ShieldComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UCharacterAnimationSet> AnimSet;

	UFUNCTION(BlueprintCallable, Category = "FSM")
	virtual void RegisterCustomStates(UFighterStateMachine* FSM){}
	
	bool SetCurrentAnimation(EAnimSlot Anim, int32 BlendTime = 0);

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
	
	const FAttackDefinition* DetermineAttack(EInputButton InputButton, bool bFlickInput, EStickDir StickDir) const;

	bool TryStartAttack(EInputButton Button, FFighterInput* Input);

	UFUNCTION(BlueprintCallable, Category = "Damage")
	void ApplyDamage(int32 Damage);

	UFUNCTION(BlueprintCallable, Category = "Damage")
	void FaceInstigator();

	UFUNCTION(BlueprintCallable, Category = "Damage")
	void SetStocks(int32 StockCount);

	UFUNCTION(BlueprintCallable, Category = "Damage")
	void LoseStock();

	virtual void WasHit(const FDamageInfo& DamageInfo, AFighterPawn* Instigator) override;

	virtual FVector CenterLocation() override {return Fixed2DToVector(MovementComponent->GetCenter());}
	
	float GetBakedBoneRotation(FName BoneName);
	FBakedSocketKey GetBakedSocketKey(FName SocketName);
	FFixedVector2D GetBakedSocketLocation(FName SocketName);
	
	void ShakeMesh();
	void ResetMeshOffset();
	
	void InitiateKnockback();
	void HandleGrab();
	void HandleParry();

	UPROPERTY(BlueprintReadWrite, Category = "CharStats")
	int32 Weight = 100;

	void SetCharacterPanel(UCharacterPanelWidget* Panel) { CharacterPanel = Panel; }

	FFixedVector2D GetFixedLoc() const { return VectorToFixed2D(GetActorLocation()); }
	void SetFixedLoc(FFixedVector2D InLoc) { SetActorLocation(Fixed2DToVector(InLoc)); }

	UPROPERTY(EditAnywhere, Instanced, Category="Projectile")
	TArray<UProjectilePool*> ProjectilePools;

protected:

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
};