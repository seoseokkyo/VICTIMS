
#pragma once

#include "CoreMinimal.h"
#include "EEquipmentSlot.h"
#include "EItemQuality.h"
#include "EItemType.h"
#include "Engine/DataTable.h"
#include "FItemStructure.generated.h"



USTRUCT(BlueprintType)
struct VICTIMS_API FItemStructure : public FTableRowBase
{
	GENERATED_BODY()

public:
	FItemStructure();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Icon;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EItemType ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EItemQuality Quality;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsInShop;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 PriceValue;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsStackable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxStackSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UClass* Class;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsDroppable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMesh* SkeletalMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* WorldMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Duration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEquipmentSlot EquipmentSlot;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Strength;
        	
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Health;
};
