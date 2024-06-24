// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class VICTIMS_API ICombatInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ContinueAttack();
	virtual void ContinueAttack_Implementation() PURE_VIRTUAL(ICombatInterface::ContinueAttack_Implementation, return;);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ResetAttack();
	virtual void ResetAttack_Implementation() PURE_VIRTUAL(ICombatInterface::ResetAttack_Implementation, return;);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FRotator GetDesireRotation();
	virtual FRotator GetDesireRotation_Implementation() PURE_VIRTUAL(ICombatInterface::GetDesireRotation_Implementation, return FRotator::ZeroRotator;);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ResetCombat();
	virtual void ResetCombat_Implementation() PURE_VIRTUAL(ICombatInterface::ResetCombat_Implementation, return;);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool CanReceiveDamage();
	virtual bool CanReceiveDamage_Implementation() PURE_VIRTUAL(ICombatInterface::CanReceiveDamage_Implementation, return bool(false););
};
