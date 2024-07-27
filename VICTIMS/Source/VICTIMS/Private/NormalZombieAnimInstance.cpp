// Fill out your copyright notice in the Description page of Project Settings.


#include "NormalZombieAnimInstance.h"
#include "NormalZombie.h"

void UNormalZombieAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if (auto owningActor = Cast<ANormalZombie>(GetOwningActor()))
	{
		bDead = owningActor->bDead;
	}
}
