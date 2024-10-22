// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CombatInterface.h"
#include "StateComponent.h"
#include "CharacterBase.generated.h"

UENUM()
enum class ECharacterType : int8
{
	PlayerCharacter,
	Enemy
};

UENUM()
enum class ECharacterMotionState : int8
{
	Idle,
	ToggleCombat,
	Attack,
	Defence,
	Hit,
	Jump,
	Die,
	Move,
	AttackDelay,
	RETURN
};

class UAnimMontage;
class UCombatComponent;

DECLARE_DELEGATE_OneParam(FEndAttackEvent, float);

UCLASS()
class VICTIMS_API ACharacterBase : public ACharacter, public ICombatInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacterBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString characterName;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "MySettings")
	bool bDead;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "MySettings")
	UStateComponent* stateComp;
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Components")
	UCombatComponent* combatComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MySettings")
	ECharacterType characterType;
	
	UPROPERTY(EditDefaultsOnly, Category = "MySettings")
	UAnimMontage* hitReaction;	

	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadWrite)
	ECharacterMotionState motionState = ECharacterMotionState::Idle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	USoundBase* DeathSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<USoundBase*> HitSounds;

	virtual void ContinueAttack_Implementation() override;
	virtual void ResetCombat_Implementation() override;
	virtual bool CanReceiveDamage_Implementation() override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bIsDead=false;

	void PrintInfo();

	UFUNCTION(Server, Reliable)
	void ServerRPC_HitReact();

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticastRPC_HitReact();

	FName pelvisBoneName = TEXT("Pelvis");

	UFUNCTION(BlueprintCallable)
	void AttackEvent();

	UFUNCTION(BlueprintCallable)
	void PerformAttack(int32 attackIndex, bool bUseRandom);

	void EnableRagdoll();

	FString GetName() {return characterName;};

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable)
	void ServerRPC_PerformAttack(int useIndex);

	UFUNCTION(NetMulticast, Reliable)
	virtual void NetMulticastRPC_PerformAttack(int useIndex);

	UFUNCTION(Server, Reliable)
	void ServerRPC_AmountDamage(float damage);

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticastRPC_AmountDamage(float currentHP);

	UFUNCTION()
	virtual void DieFunction();

	UFUNCTION(BlueprintImplementableEvent)
	void AfterDieFunction();

	UFUNCTION(Server, Reliable)
	void ServerRPC_ToIdle();

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticastRPC_ToIdle(ECharacterMotionState state);

	FEndAttackEvent OnEndAttackEvent;

	UFUNCTION()
	bool TeamCheck(ACharacterBase* targetActor);
};
