// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWeapon.h"
#include <GameFramework/Character.h>
#include <Kismet/GameplayStatics.h>
#include "CollisionComponent.h"
#include "CombatComponent.h"
#include "CombatInterface.h"
#include "AnimInstance_Interface.h"


ABaseWeapon::ABaseWeapon()
{
	collisionComponent = CreateDefaultSubobject<UCollisionComponent>(TEXT("CollisionComponent"));
	ItemStaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (collisionComponent != nullptr)
	{
		collisionComponent->onHitDeligate.BindUObject(this, &ABaseWeapon::OnHitCollisionComponent);
	}
	else
	{
		int iTemp = 0;
	}
}

void ABaseWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseWeapon::OnEquipped()
{
	//Super::OnEquipped();
	if (GetOwner() != nullptr)
	{
		combatComponent = GetOwner()->GetComponentByClass<UCombatComponent>();
		if (combatComponent != nullptr)
		{
			FName selectedName = (combatComponent->bCombatEnable ? handSocketName : AttachSocketName);

			AttachActor(selectedName);

			combatComponent->SetMainWeapon(this);

			ACharacter* characterPtr = Cast<ACharacter>(GetOwner());
			if (characterPtr)
			{
				USkeletalMeshComponent* skeletalMeshComponent = characterPtr->GetMesh();
				if (skeletalMeshComponent)
				{
					UAnimInstance* animInstance = skeletalMeshComponent->GetAnimInstance();

					if (animInstance != nullptr)
					{
						IAnimInstance_Interface* AnimInterface = Cast<IAnimInstance_Interface>(animInstance);

						if (AnimInterface != nullptr)
						{
							AnimInterface->UpdateCombatType_Implementation(eWeaponType);

							UE_LOG(LogTemp, Warning, TEXT("%s Send UUpdateCombatType"), *animInstance->GetName());
						}
						else
						{
							UE_LOG(LogTemp, Warning, TEXT("%s Is do not Have IAnimInstance_Interface"), *skeletalMeshComponent->GetName());
						}
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("skeletalMeshComponent Is Nullptr"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Owner Is Not Character"));
			}

			collisionComponent->SetCollisionMesh(GetItemMesh());
			collisionComponent->AddActorToIgnore(GetOwner());
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("selectedName : %p"), this);
	}
}

void ABaseWeapon::OnEquippedTarget(UCombatComponent* combatcomp)
{
	if (GetOwner() != nullptr)
	{
		if (combatcomp != nullptr)
		{
			FName selectedName = (combatcomp->bCombatEnable ? handSocketName : AttachSocketName);

			AttachActor(selectedName);

			combatcomp->SetMainWeapon(this);

			ACharacter* characterPtr = Cast<ACharacter>(GetOwner());
			if (characterPtr)
			{
				USkeletalMeshComponent* skeletalMeshComponent = characterPtr->GetMesh();
				if (skeletalMeshComponent)
				{
					UAnimInstance* animInstance = skeletalMeshComponent->GetAnimInstance();

					if (animInstance != nullptr)
					{
						IAnimInstance_Interface* AnimInterface = Cast<IAnimInstance_Interface>(animInstance);

						if (AnimInterface != nullptr)
						{
							AnimInterface->UpdateCombatType_Implementation(eWeaponType);

							UE_LOG(LogTemp, Warning, TEXT("%s Send UUpdateCombatType"), *animInstance->GetName());
						}
						else
						{
							UE_LOG(LogTemp, Warning, TEXT("%s Is do not Have IAnimInstance_Interface"), *skeletalMeshComponent->GetName());
						}
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("skeletalMeshComponent Is Nullptr"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Owner Is Not Character"));
			}

			collisionComponent->SetCollisionMesh(GetItemMesh());
			collisionComponent->AddActorToIgnore(GetOwner());
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("selectedName : %p"), this);
	}
}

void ABaseWeapon::OnHitCollisionComponent(FHitResult lastHitStruct)
{
	UE_LOG(LogTemp, Warning, TEXT("OnHitCollisionComponent Called"));

	AActor* hitActor = lastHitStruct.GetActor();

	auto interfaceCheck = Cast<ICombatInterface>(hitActor);

	if (interfaceCheck != nullptr)
	{
		interfaceCheck->CanReceiveDamage_Implementation();

		FVector hitFromDirection = GetOwner()->GetActorForwardVector();

		TSubclassOf<UDamageType> damageTypeClass = {};

		//<< �̰� damageTypeClass�� �������Ʈ�� �⺻���̶� ���̰� �ִ��� Ȯ���ʿ�
		UGameplayStatics::ApplyPointDamage(hitActor, weaponDamage, hitFromDirection, lastHitStruct, GetInstigatorController(), this, damageTypeClass);
	}
}

void ABaseWeapon::SimulateWeaponPhysics()
{
	GetItemMesh()->SetCollisionProfileName(FName("PhysicsActor"));
	GetItemMesh()->SetSimulatePhysics(true);
}