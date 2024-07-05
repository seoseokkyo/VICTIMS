// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAnimInstance.h"
#include "CharacterBase.h"

void UCharacterAnimInstance::UpdateCombatType_Implementation(EWeaponType eType)
{
	weaponType = eType;
}

void UCharacterAnimInstance::UpdateCombatEnabled_Implementation(bool bEnable)
{
	bCombatEnable = bEnable;
}

void UCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	myCharacter = Cast<ACharacterBase>(GetOwningActor());
}

void UCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if (myCharacter != nullptr)
	{
		bDie = myCharacter->bDead;
	}
}

