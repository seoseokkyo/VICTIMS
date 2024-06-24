// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "BaseWeapon.h"
#include <GameFramework/Character.h>
#include "AnimInstance_Interface.h"
#include <Net/UnrealNetwork.h>
#include <Kismet/KismetSystemLibrary.h>

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...

	ComponentTags.Add(TEXT("CombatComp"));
}


// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	// ...
	UE_LOG(LogTemp, Warning, TEXT("CombatComp GetIsReplicated() : %s"), GetIsReplicated() ? TEXT("TRUE") : TEXT("FALSE"));
	if (GetIsReplicated() != true)
	{
		SetIsReplicated(true);
	}
}


// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void UCombatComponent::SetMainWeapon(ABaseWeapon* baseWeapon)
{
	if (IsValid(mainWeapon))
	{
		mainWeapon->OnUnEquipped();
		mainWeapon->Destroy();
	}

	mainWeapon = baseWeapon;
}

ABaseWeapon* UCombatComponent::GetMainWeapon()
{
	return mainWeapon;
}

void UCombatComponent::SetCombatEnabled(bool bEnable)
{
	bCombatEnable = bEnable;

	/*
	*	1. GetOwner()
	*	2. CastToCharacter
	*	3. Character에서 Mesh추출
	*	4. Mesh에서 GetAnimInstance
	*	5. GetAnimInstance로 IAnimInstance_Interface변환이 가능할 경우 UpdateCombatEnabled호출
	*/
	ACharacter* player = Cast<ACharacter>(GetOwner());
	if (player != nullptr)
	{
		USkeletalMeshComponent* meshComp = player->GetMesh();
		if (meshComp != nullptr)
		{
			IAnimInstance_Interface* animInterface = Cast<IAnimInstance_Interface>(meshComp->GetAnimInstance());
			if (animInterface != nullptr)
			{
				animInterface->UpdateCombatEnabled_Implementation(bCombatEnable);
			}
		}
	}
}

bool UCombatComponent::CheckCombatEnabled()
{
	return bCombatEnable;
}

void UCombatComponent::ResetAttack()
{
	attackCount = 0;
	bAttackSaved = false;
	bAttacking = false;
}


void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, bCombatEnable);
	DOREPLIFETIME(UCombatComponent, attackCount);
	DOREPLIFETIME(UCombatComponent, bAttackSaved);
	DOREPLIFETIME(UCombatComponent, bAttacking);
}

