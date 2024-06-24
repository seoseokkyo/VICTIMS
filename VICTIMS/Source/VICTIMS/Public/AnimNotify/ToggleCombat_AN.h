// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "ToggleCombat_AN.generated.h"

/**
 * 
 */
UCLASS()
class VICTIMS_API UToggleCombat_AN : public UAnimNotify
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable)
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;	
};
