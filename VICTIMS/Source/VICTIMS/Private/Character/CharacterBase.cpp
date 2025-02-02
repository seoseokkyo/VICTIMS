// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterBase.h"
#include <Engine/DamageEvents.h>
#include <Kismet/KismetSystemLibrary.h>
#include "Animation/AnimMontage.h"
#include "BaseWeapon.h"
#include "StateComponent.h"
#include "CombatComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <Net/UnrealNetwork.h>
#include "VICTIMSCharacter.h"
#include <Kismet/GameplayStatics.h>
#include "AVICTIMSPlayerController.h"

// Sets default values
ACharacterBase::ACharacterBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	stateComp = CreateDefaultSubobject<UStateComponent>(TEXT("StateComp"));

	combatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComp"));

	characterName = TEXT("Default");
}

// Called when the game starts or when spawned
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();

	//UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("characterName : %s"), *characterName));

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	stateComp->InitStat();
	stateComp->UpdateStat();
	stateComp->dieDelegate.BindUFunction(this, FName("DieFunction"));

	bDead = false;
}

// Called every frame
void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (0)
	{
		PrintInfo();
	}
}

// Called to bind functionality to input
void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float ACharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bDead)
	{
		return 0.0f;
	}

	bool bSameTeam = false;

	if (TeamCheck(Cast<ACharacterBase>(DamageCauser)))
	{
		bSameTeam = true;
	}

	if (auto playerCheck = Cast<AVICTIMSPlayerController>(EventInstigator))
	{
		if (auto charCheck = playerCheck->GetPawn())
		{
			if (TeamCheck(Cast<ACharacterBase>(charCheck)))
			{
				bSameTeam = true;
			}
		}
	}

	if (bSameTeam)
	{
		return 0.0f;
	}


	float temp = stateComp->AddStatePoint(HP, -DamageAmount);
	stateComp->NetMulticastRPC_SetStatePoint(HP, temp);

	if (stateComp->GetStatePoint(HP) <= 0)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("%s is Dead By %s"), *this->GetActorNameOrLabel(), *DamageCauser->GetActorNameOrLabel()), true, true, FLinearColor::Red, 10.0f);

		if (bIsDead == false)
		{
			bIsDead = true;
			//UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("%s is Dead By %s"), *this->GetActorNameOrLabel(), *DamageCauser->GetActorNameOrLabel()), true, true, FLinearColor::Red, 1000.0f);
			AfterDieFunction();

			AVICTIMSCharacter* Attacker = Cast<AVICTIMSCharacter>(DamageCauser);
			if (Attacker != nullptr)
			{
				Attacker->NetMulticastRPC_KillWidget(this);
				//UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("WidgetOn")), true, true, FLinearColor::Red, 10.0f);
			}

		}
	}

	if (HitSounds.Num() > 0)
	{
		int selected = FMath::RandRange(0, HitSounds.Num() - 1);

		UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSounds[selected], GetActorLocation());
	}

	if (hitReaction)
	{
		motionState = ECharacterMotionState::Hit;

		FVector targetDir = DamageCauser->GetActorLocation() - GetActorLocation();
		targetDir.Normalize();

		SetActorRotation(targetDir.Rotation());

		ServerRPC_HitReact();

		//float animTime = PlayAnimMontage(hitReaction, 1.0f);

		//FTimerHandle hnd;
		//GetWorldTimerManager().SetTimer(hnd, [&]() {

		//	motionState = ECharacterMotionState::Idle;

		//	}, animTime, false);
	}

	// 디버그
	if (1)
	{
		if (EventInstigator != nullptr)
		{
			UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("EventInstigator : %s"), *EventInstigator->GetActorNameOrLabel()));
		}

		if (DamageCauser != nullptr)
		{
			UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Attack By : %s"), *DamageCauser->GetActorNameOrLabel()));
		}
	}

	return 0.0f;
}

void ACharacterBase::PrintInfo()
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

	FString str = FString::Printf(TEXT("localRole : %s\nremoteRole : %s\nowner : %s\nnetConn : %s\nhasController : %s\n HP : %s\n SP : %s\n strState : %s\n"), *localRole, *remoteRole, *owner, *netConn, *hasController, *strHP, *strSP, *strState);

	FVector loc = GetActorLocation() + FVector(0, 0, 50);
	DrawDebugString(GetWorld(), loc, str, nullptr, FColor::White, 0, true);
}

void ACharacterBase::ServerRPC_HitReact_Implementation()
{
	motionState = ECharacterMotionState::Hit;

	NetMulticastRPC_HitReact();
}

void ACharacterBase::NetMulticastRPC_HitReact_Implementation()
{
	motionState = ECharacterMotionState::Hit;

	if (hitReaction)
	{
		float animTime = PlayAnimMontage(hitReaction, 1.0f);

		FTimerHandle hnd;
		GetWorldTimerManager().SetTimer(hnd, [&]() {

			ServerRPC_ToIdle();

			}, animTime, false);
	}
}

void ACharacterBase::ContinueAttack_Implementation()
{
	if (combatComponent == nullptr) return;

	combatComponent->bAttacking = false;

	if (combatComponent->bAttackSaved)
	{
		combatComponent->bAttackSaved = false;

		//UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("ContinueAttack_Implementation")));

		AttackEvent();
	}
}

void ACharacterBase::ResetCombat_Implementation()
{
	motionState = ECharacterMotionState::Idle;
	combatComponent->attackCount = 0;
	combatComponent->bAttacking = false;
	combatComponent->bAttackSaved = false;
}

bool ACharacterBase::CanReceiveDamage_Implementation()
{
	return !bDead;
}

void ACharacterBase::AttackEvent()
{
	if (combatComponent == nullptr) return;

	if (motionState == ECharacterMotionState::Idle || motionState == ECharacterMotionState::Attack)
	{
		PerformAttack(combatComponent->attackCount, false);
	}
}

void ACharacterBase::PerformAttack(int32 attackIndex, bool bUseRandom)
{
	ABaseWeapon* mainWeapon = nullptr;
	UAnimMontage* useMontage = nullptr;

	if (combatComponent)
	{
		mainWeapon = combatComponent->GetMainWeapon();

		if (mainWeapon != nullptr)
		{
			// 랜덤 인덱스 생성
			int32 montagesSize = mainWeapon->attackMontages.Num();
			int32 randIndex = FMath::RandRange(0, montagesSize - 1);

			int useIndex = bUseRandom ? randIndex : attackIndex;

			useMontage = bUseRandom ? mainWeapon->attackMontages[randIndex] : mainWeapon->attackMontages[attackIndex];

			if (IsValid(useMontage))
			{
				ServerRPC_PerformAttack(useIndex);
			}
		}
	}
}

void ACharacterBase::EnableRagdoll()
{
	if (GetCharacterMovement())
	{
		//GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None, 0);

		if (GetCapsuleComponent())
		{
			GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
			GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

			FAttachmentTransformRules attachTransformRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true);

			auto TestComps = GetComponentsByTag(USkeletalMeshComponent::StaticClass(), (TEXT("Ragdoll")));
			for (auto test : TestComps)
			{
				if (auto skeletal = Cast<USkeletalMeshComponent>(test))
				{
					skeletal->SetCollisionProfileName(TEXT("ragdoll"));
					skeletal->SetAllBodiesBelowSimulatePhysics(pelvisBoneName, true, true);
					skeletal->SetAllBodiesBelowPhysicsBlendWeight(pelvisBoneName, 1.0f, false, true);
				}
			}

			if (GetMesh())
			{
				GetMesh()->SetCollisionProfileName(TEXT("ragdoll"));
				GetMesh()->SetAllBodiesBelowSimulatePhysics(pelvisBoneName, true, true);
				GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(pelvisBoneName, 1.0f, false, true);
			}

			//if (CameraBoom)
			//{
			//	if (GetMesh())
			//	{
			//		CameraBoom->AttachToComponent(GetMesh(), attachTransformRules, pelvisBoneName);
			//		CameraBoom->bDoCollisionTest = false;

			//		GetMesh()->SetCollisionProfileName(TEXT("ragdoll"));
			//		GetMesh()->SetAllBodiesBelowSimulatePhysics(pelvisBoneName, true, true);
			//		GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(pelvisBoneName, 1.0f, false, true);
			//	}
			//	else
			//	{
			//		UE_LOG(LogTemp, Warning, TEXT("AEromCharacter : %d"), __LINE__);
			//	}
			//}
			//else
			//{
			//	UE_LOG(LogTemp, Warning, TEXT("AEromCharacter : %d"), __LINE__);
			//}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("AEromCharacter : %d"), __LINE__);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AEromCharacter : %d"), __LINE__);
	}
}

void ACharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACharacterBase, stateComp);
	DOREPLIFETIME(ACharacterBase, combatComponent);
	DOREPLIFETIME(ACharacterBase, motionState);
	DOREPLIFETIME(ACharacterBase, bDead);
}

void ACharacterBase::ServerRPC_PerformAttack_Implementation(int useIndex)
{
	ABaseWeapon* mainWeapon = combatComponent->GetMainWeapon();

	combatComponent->bAttacking = true;

	motionState = ECharacterMotionState::Attack;

	// 여기나 다른데서 캐릭터 스탯 및 무기 데미지 등 적용
	mainWeapon->weaponDamage = mainWeapon->weaponDamage;

	// 카운트 증가
	combatComponent->attackCount++;

	int32 montageLastIndex = mainWeapon->attackMontages.Num() - 1;

	// 카운트 초기화
	if (combatComponent->attackCount > montageLastIndex)
	{
		combatComponent->attackCount = 0;
	}

	NetMulticastRPC_PerformAttack(useIndex);
}

void ACharacterBase::NetMulticastRPC_PerformAttack_Implementation(int useIndex)
{
	ABaseWeapon* mainWeapon = combatComponent->GetMainWeapon();

	if (mainWeapon)
	{
		combatComponent->bAttacking = true;

		float attackAnimTime = PlayAnimMontage(mainWeapon->attackMontages[useIndex]);

		OnEndAttackEvent.ExecuteIfBound(attackAnimTime);

		//FTimerHandle handler;
		//GetWorldTimerManager().SetTimer(handler, [&]() {

		//	combatComponent->bAttacking = false;
		//	combatComponent->bAttackSaved = false;
		//	motionState = ECharacterMotionState::Idle;

		//	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("NetMulticastRPC_PerformAttack_Implementation")));

		//	GetWorldTimerManager().ClearTimer(handler);

		//	}, 1.0f, false, attackAnimTime);
	}
}

void ACharacterBase::ServerRPC_AmountDamage_Implementation(float damage)
{
	if (stateComp != nullptr)
	{
		stateComp->AddStatePoint(HP, -damage);
	}
}

void ACharacterBase::NetMulticastRPC_AmountDamage_Implementation(float damage)
{
	if (stateComp != nullptr)
	{
		stateComp->AddStatePoint(HP, -damage);
	}
}

void ACharacterBase::DieFunction()
{
	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Dead : %s"), *GetActorNameOrLabel()));

	bDead = true;

}

bool ACharacterBase::TeamCheck(ACharacterBase* targetActor)
{
	if (targetActor == nullptr)
	{
		return false;
	}

	// Enemy : Enemy Check
	FString ownerTeamTag;
	FString hitActorTeamTag;

	bool bOwnerTagFind = false;
	for (auto tag : Tags)
	{
		if (tag.ToString().Contains("Team"))
		{
			ownerTeamTag = tag.ToString();
			bOwnerTagFind = true;
			break;
		}
	}

	bool bHitActorTagFind = false;
	for (auto tag : targetActor->Tags)
	{
		if (tag.ToString().Contains("Team"))
		{
			hitActorTeamTag = tag.ToString();
			bHitActorTagFind = true;
			break;
		}
	}

	if (bOwnerTagFind && bHitActorTagFind && (ownerTeamTag == hitActorTeamTag))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void ACharacterBase::ServerRPC_ToIdle_Implementation()
{
	motionState = ECharacterMotionState::Idle;

	NetMulticastRPC_ToIdle(ECharacterMotionState::Idle);
}

void ACharacterBase::NetMulticastRPC_ToIdle_Implementation(ECharacterMotionState state)
{
	motionState = state;
}

