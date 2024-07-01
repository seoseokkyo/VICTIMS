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

// Sets default values
ANormalZombie::ANormalZombie()
{
	PrimaryActorTick.bCanEverTick = true;

	characterName = TEXT("NormalZombie");
}


void ANormalZombie::BeginPlay()
{
	Super::BeginPlay();

	// 월드에 있는 플레이어를 찾는다.
	for (TActorIterator<AVICTIMSCharacter> findActor(GetWorld()); findActor; ++findActor)
	{
		Player = *findActor;
	}

	aiCon = GetAIController();

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

	switch (motionState)
	{
	case ECharacterMotionState::Idle:
		Idle();
		break;
	case ECharacterMotionState::ToggleCombat:
		break;
	case ECharacterMotionState::Attack:
		Attack();
		break;
	case ECharacterMotionState::Defence:
		break;
	case ECharacterMotionState::Hit:
		break;
	case ECharacterMotionState::Jump:
		break;
	case ECharacterMotionState::Die:
		if (bDead == false)
		{
			Die();
		}
		break;
	case ECharacterMotionState::Move:
		MoveTotaget();
		break;
	case ECharacterMotionState::AttackDelay:
		AttackDelay();
		break;
	case ECharacterMotionState::RETURN:

		break;
	default:
		break;
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

void ANormalZombie::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ANormalZombie::Idle()
{
	SearchPlayer();
}

void ANormalZombie::MoveTotaget()
{
	targetLoc = Player->GetActorLocation();
	if (aiCon != nullptr)
	{
		if (FVector::Distance(GetActorLocation(), targetLoc) < limitDistance && FVector::Distance(GetActorLocation(), targetLoc) > attackDistance)
		{
			UE_LOG(LogTemp, Warning, TEXT("move!!"));
			aiCon->SetFocus(Player);
			aiCon->MoveToActor(Player);
		}
		if (FVector::Distance(GetActorLocation(), targetLoc) <= attackDistance)
		{
			motionState = ECharacterMotionState::Attack;
		}

		if (FVector::Distance(GetActorLocation(), targetLoc) >= 500)
		{
			motionState = ECharacterMotionState::Idle;
		}
	}
}

void ANormalZombie::Attack()
{
	// 공격전 플레이어에게로 방향회전
	FVector lookDir = Player->GetActorLocation() - GetActorLocation();
	moveDir = lookDir.GetSafeNormal();  //노멀라이즈를 해줘야 백터 길이가 1이됨.
	FRotator newRot = UKismetMathLibrary::MakeRotFromZX(GetActorUpVector(), moveDir);  // 첫번째 축은 고정축 두번째 축은 맞추려는 축
	rotStart = GetActorRotation();
	rotTarget = newRot;
	bLookTarget = true;

	/*UE_LOG(LogTemp, Warning, TEXT("Attack!!"));*/
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	bAttack = true;

	bOnAttackDelay = false;

	if (AnimInstance && bAttack)
	{
		PlayAnimMontage(attack_Montage);

		motionState = ECharacterMotionState::AttackDelay;
	}
}

void ANormalZombie::AttackDelay()
{
	if (bOnAttackDelay == false)
	{
		aiCon = GetAIController();

		if (aiCon != nullptr)
		{
			aiCon->StopMovement();
		}

		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("AttackDelay Call"));

		FTimerHandle AttackTimer;

		bOnAttackDelay = true;

		GetWorld()->GetTimerManager().SetTimer(AttackTimer, [&]() {

			motionState = ECharacterMotionState::Idle;

			UKismetSystemLibrary::PrintString(GetWorld(), TEXT("EEnemyState::IDLE"));

			}, 6, false);
	}
}

void ANormalZombie::DieFunction()
{
	GetMesh()->GetAnimInstance()->StopAllMontages(0.2);

	Super::DieFunction();

	if (DeathSound && motionState != ECharacterMotionState::Die)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeathSound, GetActorLocation());
	}

	ServerRPC_DieFunction();
}

void ANormalZombie::ServerRPC_DieFunction_Implementation()
{
	motionState = ECharacterMotionState::Die;

	aiCon->SetFocus(nullptr);

	FTimerHandle hnd;
	GetWorldTimerManager().SetTimer(hnd, [&]() {

		EnableRagdoll();

		}, 2.0f, false);

	NetMulticastRPC_DieFunction();
}

void ANormalZombie::NetMulticastRPC_DieFunction_Implementation()
{
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

void ANormalZombie::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANormalZombie, motionState);
}

AAIController* ANormalZombie::GetAIController()
{
	aiCon = GetController<AAIController>();

	if (aiCon == nullptr)
	{
		SpawnDefaultController();
		aiCon = GetController<AAIController>();
	}

	return aiCon;
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

	FString str = FString::Printf(TEXT("localRole : %s\nremoteRole : %s\nowner : %s\nnetConn : %s\nnetMode : %s\nhasController : %s\n HP : %s\n SP : %s\n strState : %s"), *localRole, *remoteRole, *owner, *netConn, /**netMode,*/ *hasController, *strHP, *strSP, *strState);

	FVector loc = GetActorLocation() + FVector(0, 0, 50);
	DrawDebugString(GetWorld(), loc, str, nullptr, FColor::White, 0, true);
}



//void ANormalZombie::OnDamaged(int32 dmg)
//{
//	EnemyCurrentHP = FMath::Clamp(EnemyCurrentHP - dmg, 0, 100);
//	if (EnemyCurrentHP <= 0)
//	{
//		enemyState = EEnemyState::DIE;
//
//
//	}
//	else
//	{
//		//enemyState = EEnemyState::DAMAGED;
//		//UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
//		//if (AnimInstance)
//		//{
//		//	PlayAnimMontage(HitReact_Montage);
//		//	UE_LOG(LogTemp, Warning, TEXT("HitReactAM!!"))
//		//}
//	}
//
//}

void ANormalZombie::SearchPlayer()
{
	//UE_LOG(LogTemp, Warning, TEXT("sug"));

	Player = nullptr;

	float nearestDist = 9999999.9f;

	// 월드에 있는 플레이어를 찾는다.
	for (TActorIterator<AVICTIMSCharacter> findActor(GetWorld()); findActor; ++findActor)
	{
		AVICTIMSCharacter* temp = nullptr;

		temp = *findActor;

		FVector Start = GetActorLocation();
		FVector End = temp->GetActorLocation();

		float dist = FVector::Dist(Start, End);

		if (dist < 1000)
		{
			if (nearestDist > dist)
			{
				nearestDist = dist;

				Player = *findActor;
			}
		}
	}

	if (Player == nullptr)
	{
		motionState = ECharacterMotionState::Idle;
	}
	else
	{
		motionState = ECharacterMotionState::Move;
	}
}



