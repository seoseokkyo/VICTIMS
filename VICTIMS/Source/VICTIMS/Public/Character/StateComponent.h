// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StateComponent.generated.h"


UENUM()
enum EStateType
{
	HP,
	SP
};



// DataTable(베이스 포인트)
USTRUCT(BlueprintType)
struct FCharacterStat : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float MaxHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float MaxSP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	int32 Strength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	int32 Agility;
};

USTRUCT(BlueprintType)
struct FCharacterRunningStat
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MySettings")
	FCharacterStat DefinedStat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MySettings")
	float currentHP = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MySettings")
	float MaxHP = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MySettings")
	float currentSP = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MySettings")
	float MaxSP = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MySettings")
	int32 currentStrength = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MySettings")
	int32 currentAgility = 0;
};

class ACharacterBase;
class UDataTable;

DECLARE_DYNAMIC_DELEGATE(FCharacterHPZero);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterStatePointChanged, FCharacterRunningStat, characterRunningStat);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VICTIMS_API UStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStateComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
private:

	ACharacterBase* character;

public:

	UPROPERTY(ReplicatedUsing = "OnRep_bEnableReady")
	bool bEnableReady = false;

	UFUNCTION(meta = (OverrideNativeName = "OnRep_bEnableReady"))
	void OnRep_bEnableReady(bool bEnable);

	UFUNCTION(Server, Reliable)
	void ServerRPC_EnableReady(bool bEnable);

	UFUNCTION(Client, Reliable)
	void ClientRPC_EnableReady(bool bEnable);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MySettings")
	FCharacterStat stat;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "MySettings")
	FCharacterRunningStat runningStat;

	void InitStat();

	// Get Current Value
	UFUNCTION(BlueprintCallable)
	float GetStatePoint(EStateType stateType);

	UFUNCTION(BlueprintCallable)
	float AddStatePoint(EStateType stateType, float value);


	UFUNCTION(Server, Reliable)
	void ServerRPC_SetStatePoint(EStateType stateType, float value);

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticastRPC_SetStatePoint(EStateType stateType, float value);

	int32 GetStrength() { return runningStat.currentStrength; };
	int32 GetAgility() { return runningStat.currentAgility; };

	// Set CurrentValue
	void UpdateStat();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintReadWrite, Category = "MySettings")
	FCharacterHPZero dieDelegate;	

	UPROPERTY(BlueprintReadWrite, Category = "MySettings")
	FCharacterStatePointChanged OnCharacterRunningStatChanged;
};
