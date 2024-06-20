// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Engine/DataTable.h>
#include "ItemBase.generated.h"

class UDataTable;
class UTexture2D;
class UStaticMesh;

UENUM()
enum class EItemNumber : uint8
{
	MaxEItemNumber
};

UENUM()
enum class EItemTYPE : uint8
{
	Stuff,
	Equipable,
	Consumable_Direct,
	Consumable_Indirect,
	Buildable,
	EItemTYPE
};

UENUM()
enum class EItemMatter : uint8
{
	Wood,
	Stone,
	Metal,
	EItemMaterialType
};

//USTRUCT(BlueprintType)
//struct FItemBaseUsingUIData : public FTableRowBase
//{
//	GENERATED_BODY()
//
//
//};

USTRUCT(BlueprintType)
struct FItemBaseData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	EItemNumber				itemNumber;

	UPROPERTY(BlueprintReadWrite)
	EItemTYPE				itemType;

	UPROPERTY(BlueprintReadWrite)
	EItemMatter				itemMatterType;

	UPROPERTY(BlueprintReadWrite)
	UTexture2D*				itemTexture;

	UPROPERTY(BlueprintReadWrite)
	UStaticMesh*			itemMesh;

	UPROPERTY(BlueprintReadWrite)
	FString					itemDescription;

	UPROPERTY(BlueprintReadWrite)
	int32					itemOriginPrice;

	UPROPERTY(BlueprintReadWrite)
	int32					itemDurability;

	UPROPERTY(BlueprintReadWrite)
	int32					itemCustomValue1;

	UPROPERTY(BlueprintReadWrite)
	int32					itemCustomValue2;

	//UPROPERTY(BlueprintReadWrite)
	//FItemBaseUsingUIData	usingUIData;

	UPROPERTY(BlueprintReadWrite)
	bool bUseable;

	UPROPERTY(BlueprintReadWrite)
	bool bStackable;

	UPROPERTY(BlueprintReadWrite)
	int32 currentStackCount;

	UPROPERTY(BlueprintReadWrite)
	int32 MaxStackCount;
};

UCLASS()
class VICTIMS_API AItemBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
