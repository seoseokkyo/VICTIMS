
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemDataStructs.h"
#include "EEquipmentSlot.h"
#include "FSlotStructure.generated.h"

struct FReturnTupleBoolInt
{
	bool Success = true; int8 Index = -1;
};

struct FReturnTupleSlotNativeID {
	bool NativeFromInventory = false;
	bool NativeFromContainer = false;
	uint8 Index = 0;
};

USTRUCT(BlueprintType)
struct VICTIMS_API FSlotStructure : public FTableRowBase
{
	GENERATED_BODY();

public:
	FSlotStructure();

	void InitSlot(const FItemData& inItemStructure, const uint8& inAmount);		// 슬롯 값 세팅 및 생성 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FItemData itemStructure;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	uint8 amount;
};
