
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "FItemStructure.h"
#include "FSlotStructure.generated.h"

UENUM(BlueprintType)
enum class ESlotType : uint8
{
	Inventory				UMETA(DisplayName = "Inventory Slot"),   // 인벤토리 창
	Equipment				UMETA(DisplayName = "Equipment Slot"),   // 장비 창
	Hotbar					UMETA(DisplayName = "Hotbar Slot"),      // 퀵슬롯 창
	Container				UMETA(DisplayName = "Container Slot"),   // 보관함 창
	Loot					UMETA(DisplayName = "Loot Actor Slot"),  // 파밍(루트) 창
	Shop					UMETA(DisplayName = "Shopt Actor Slot")
};

enum class EAttributes : int8;
USTRUCT(BlueprintType)
struct VICTIMS_API FSlotStructure : public FTableRowBase
{
	GENERATED_BODY();

	FSlotStructure();

	void InitSlot(const FItemStructure& InItemStructure, const uint8& InAmount);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	FItemStructure ItemStructure;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	uint8 Amount;

	uint8 GetAttributeValueByAttribute(EAttributes Attribute);

	UPROPERTY()
	uint8 slot;
};