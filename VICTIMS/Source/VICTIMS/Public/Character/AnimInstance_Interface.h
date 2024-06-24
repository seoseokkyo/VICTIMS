// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BaseWeapon.h"
#include "AnimInstance_Interface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UAnimInstance_Interface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class VICTIMS_API IAnimInstance_Interface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void UpdateCombatType(EWeaponType eType);
	virtual void UpdateCombatType_Implementation(EWeaponType eType) PURE_VIRTUAL(IAnimInstanceInterface::UpdateCombatType_Implementation, return;);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void UpdateCombatEnabled(bool bEnable);
	virtual void UpdateCombatEnabled_Implementation(bool bEnable) PURE_VIRTUAL(IAnimInstanceInterface::UpdateCombatEnabled_Implementation, return;);
};
