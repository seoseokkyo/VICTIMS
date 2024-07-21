// Fill out your copyright notice in the Description page of Project Settings.


#include "NormalZombie.h"
#include <EngineUtils.h>
#include "Player/VICTIMSCharacter.h"
#include <Net/UnrealNetwork.h>
#include <AIController.h>
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetSystemLibrary.h>
#include <Kismet/KismetMathLibrary.h>
#include "Item/BaseWeapon.h"
#include "NormalZombieController.h"
#include "CombatComponent.h"

// Sets default values
ANormalZombie::ANormalZombie()
{
	PrimaryActorTick.bCanEverTick = true;

	combatComponent_Additional = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComp_Additional"));

	characterName = TEXT("NormalZombie");

	Tags.Add(FName(TEXT("Team:Enemy")));
}


void ANormalZombie::BeginPlay()
{
	Super::BeginPlay();

	// 기본 상태를 IDLE 상태로 초기화한다.
	motionState = ECharacterMotionState::Idle;

	FActorSpawnParameters spawnParam;
	spawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	spawnParam.TransformScaleMethod = ESpawnActorScaleMethod::MultiplyWithRoot;
	spawnParam.Owner = this;
	spawnParam.Instigator = this;

	ABaseWeapon* equipment = GetWorld()->SpawnActor<ABaseWeapon>(defaultWeapon, FTransform(GetActorRotation(), GetActorLocation(), FVector(1, 1, 1)), spawnParam);

	if (equipment)
	{
		equipment->OnEquipped();
	}

	ABaseWeapon* equipment_Additional = GetWorld()->SpawnActor<ABaseWeapon>(defaultWeapon_Additional, FTransform(GetActorRotation(), GetActorLocation(), FVector(1, 1, 1)), spawnParam);

	if (equipment_Additional)
	{
		equipment_Additional->OnEquippedTarget(combatComponent_Additional);
	}

	combatComponent->bCombatEnable = true;
	combatComponent_Additional->bCombatEnable = true;

	int iTemp = 0;
}

void ANormalZombie::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (0)
	{
		PrintInfo();
	}

	if (bDead)
	{
		if (motionState != ECharacterMotionState::Die)
		{
			motionState = ECharacterMotionState::Die;
		}

		return;
	}

	// 공격전 회전
	if (bLookTarget)
	{
		if (rotTime < 1.0f)
		{
			rotTime += DeltaTime;
			SetActorRotation(FMath::Lerp(rotStart, rotTarget, rotTime));
		}
		else
		{
			rotTime = 0;
			bLookTarget = false;
		}
	}
}

void ANormalZombie::DieFunction()
{
	GetMesh()->GetAnimInstance()->StopAllMontages(0.2);
	
	Super::DieFunction();

	//if (HasAuthority())
	//{
	//	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Server : %s is Dead"), *this->GetActorNameOrLabel()), true, true, //FLinearColor::Red, 10.0f);
	//}
	//else
	//{
	//	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Client : %s is Dead"), *this->GetActorNameOrLabel()), true, true, //FLinearColor::Red, 10.0f);
	//}
	
	
	
	if (DeathSound && motionState != ECharacterMotionState::Die)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeathSound, GetActorLocation());
	}

	ServerRPC_DieFunction();
}

void ANormalZombie::ServerRPC_DieFunction_Implementation()
{
	motionState = ECharacterMotionState::Die;

	GetController<ANormalZombieController>()->SetFocus(nullptr);

	NetMulticastRPC_DieFunction();
}

void ANormalZombie::NetMulticastRPC_DieFunction_Implementation()
{

	FTimerHandle hnd;
	GetWorldTimerManager().SetTimer(hnd, [&]() {

		EnableRagdoll();
		//GetController<ANormalZombieController>()->UnPossess();

		}, 2.0f, false);

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		float animTime = PlayAnimMontage(death_Montage);
		UE_LOG(LogTemp, Warning, TEXT("death_AM!!"));
	}
}

void ANormalZombie::Die()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		PlayAnimMontage(death_Montage);
		UE_LOG(LogTemp, Warning, TEXT("death_AM!!"));
	}
}

void ANormalZombie::PrintInfo()
{
	// localRole
	FString localRole = UEnum::GetValueAsString(GetLocalRole());

	// remoteRole
	FString remoteRole = UEnum::GetValueAsString(GetRemoteRole());

	// owner
	FString owner = GetOwner() ? GetOwner()->GetName() : "";

	// netConn
	FString netConn = GetNetConnection() ? "Valid" : "Invalid";

	//FString netMode = UEnum::GetValueAsString(GetNetMode());

	FString hasController = Controller ? TEXT("HasController") : TEXT("NoController");

	FString strHP = FString::Printf(TEXT("%f"), stateComp->GetStatePoint(EStateType::HP));
	FString strSP = FString::Printf(TEXT("%f"), stateComp->GetStatePoint(EStateType::SP));

	FString strState = UEnum::GetValueAsString(motionState);

	auto conTemp = GetController<ANormalZombieController>();

	float detectValue = (conTemp != nullptr) ? conTemp->GetNoiseDetect() : 0.0f;

	FString str = FString::Printf(TEXT("localRole : %s\nremoteRole : %s\nowner : %s\nnetConn : %s\nhasController : %s\n HP : %s\n SP : %s\n strState : %s\nHearingValue : %f"), *localRole, *remoteRole, *owner, *netConn, *hasController, *strHP, *strSP, *strState, detectValue);

	FVector loc = GetActorLocation() + FVector(0, 0, 50);
	DrawDebugString(GetWorld(), loc, str, nullptr, FColor::White, 0, true);
}

void ANormalZombie::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANormalZombie, motionState);
	DOREPLIFETIME(ANormalZombie, combatComponent_Additional);
}