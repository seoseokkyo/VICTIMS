// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "NormalZombieAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class VICTIMS_API UNormalZombieAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
	
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
public:
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	bool bDead = false;
};
