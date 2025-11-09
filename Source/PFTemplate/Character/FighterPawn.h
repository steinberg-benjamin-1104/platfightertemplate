#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "FighterStateMachine.h"
#include "HitboxManagerComponent.h"
#include "FighterPlayerController.h"
#include "Action.h"
#include "FrameScriptRunner.h"
#include "ShieldComponent.h"
#include "Hittable.h"
#include "CamTarget.h"
#include "FighterMovementComponent.h"
#include "BoneVectorAxis.h"

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
	void UpdateStick();
	void UpdateState();
	void InputPhase();
	void UpdateAnimation();
	void CollisionPhase();
	void ProcessCollisions();
	void ShieldPhase();
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action")
	FAction CurrentAction;

	UFUNCTION(BlueprintPure, Category = "Action")
	FAction GetCurrentAction() const {return CurrentAction; }
	
	UFUNCTION(BlueprintCallable, Category = "Action")
	bool SetCurrentAction(FName ActionName, int BlendTime = 0);

	UPROPERTY() AFighterPlayerController* FPC = nullptr;

	FStickInt8 GetPlayerStickInput() const
	{
		return FPC ? FPC->GetCurrentStickValue() : FStickInt8::ZeroVector;
	}

	bool IsFlickActive() const
	{
		return FPC ? FPC->IsFlickActive() : false;
	}

	EStickDirection GetStickDirection() const
	{
		return FPC ? FPC->GetStickDirection() : EStickDirection::Neutral;
	}

	FStickInputTracker* GetStickTracker() const
	{
		return FPC ? &FPC->StickTracker : nullptr;
	}

	UFUNCTION(BlueprintPure, Category = "Movement")
	bool IsFacingRight() const;

	UFUNCTION(BlueprintPure, Category = "Movement")
	float GetFacingDirection() const {return IsFacingRight() ? 1.f : -1.f;}

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void SetFacingDirection(float Direction);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void SwitchFacingDirection();

	UFUNCTION(BlueprintCallable, Category = "Hurtbox")
	void ToggleAllHurtboxesInvulnerable(bool bEnable);

	void ToggleHurtboxInvulnerable(FName HurtboxSuffix, bool bEnable);

	void InitHurtboxes();
	
	//Attacks
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack Data")
	UDataTable* ActionTable;
	
	FAction GetActionByName(FName MoveName) const;

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

	virtual FVector CenterLocation() override {return MovementComponent->GetCenter();}

	UFUNCTION(BlueprintPure, Category = "Mesh")
	FVector GetBoneLocation(FName BoneName) const;

	UFUNCTION(BlueprintPure, Category = "Mesh")
	FVector GetBoneVector(FName BoneName) const;

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
	
	float ShakeSign = 1.f;
};