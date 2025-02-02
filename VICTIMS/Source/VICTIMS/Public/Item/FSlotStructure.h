
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "FItemStructure.h"
#include "FSlotStructure.generated.h"


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