// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Equipable.h"
#include "BaseWeapon.generated.h"

/**
 * 
 */

class UCollisionComponent;
class UCombatComponent;
class UAnimMontage;
 
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	None = 0				UMETA(DisplayName = "None"),
	GreatSword				UMETA(DisplayName = "GreatSword"),
	E_WeaponTypeMax
};

UCLASS()
class VICTIMS_API ABaseWeapon : public AEquipable
{
	GENERATED_BODY()


public:
	ABaseWeapon();

protected:
	virtual void BeginPlay() override;


public:
	virtual void Tick(float DeltaTime) override;

public:

	// Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Initialization")
	FName handSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Initialization")
	EWeaponType				eWeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	UAnimMontage*			enterCombatMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	UAnimMontage*			exitCombatMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	TArray<UAnimMontage*>	attackMontages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	TArray<UAnimMontage*>	dodgeMontages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")

	UCollisionComponent* collisionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UCombatComponent* combatComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Initialization")
	float weaponDamage;

	// Functions
	void OnEquipped();

	void OnEquippedTarget(UCombatComponent* combatcomp);

	UFUNCTION()
	void OnHitCollisionComponent(FHitResult lastHitStruct);

	UFUNCTION(BlueprintCallable)
	void SimulateWeaponPhysics();
	
};
