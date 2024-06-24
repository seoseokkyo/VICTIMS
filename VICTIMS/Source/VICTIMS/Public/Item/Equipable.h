// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "Item/ItemBase.h"
#include "Equipable.generated.h"

/**
 * 
 */

class USceneComponent;
class USkeletalMeshComponent;
class UStaticMeshComponent;


UCLASS()
class VICTIMS_API AEquipable : public AActor
{
	GENERATED_BODY()
		
public:	
	// Sets default values for this actor's properties
	AEquipable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


private:

	bool bIsEquipped = false;

public:

	// Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	USceneComponent* rootComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	USkeletalMeshComponent* ItemSkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* ItemStaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Initialization")
	FName AttachSocketName;

	// Functions
	UFUNCTION(BlueprintCallable, category = "ItemMesh")
	UPrimitiveComponent* GetItemMesh();

	UFUNCTION(BlueprintCallable, category = "ItemMesh")
	void AttachActor(FName SocketName);

	UFUNCTION(BlueprintCallable)
	void OnEquipped();

	UFUNCTION(BlueprintCallable)
	void OnUnEquipped();

	UFUNCTION(BlueprintCallable)
	void SetIsEquipped(bool equipped);

	UFUNCTION(BlueprintCallable)
	bool EquippedCheck();	
};
