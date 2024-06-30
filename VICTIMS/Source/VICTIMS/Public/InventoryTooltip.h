
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EAttributes.h"
#include "FSlotStructure.h"
#include "ItemDataStructs.h"
#include "InventoryTooltip.generated.h"

class UUniformGridPanel;
class UTextBlock;
class UItemBase;
class UInventoryItemSlot;

UCLASS()
class VICTIMS_API UInventoryTooltip : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY()
	FSlotStructure EquippedSlotOnProfile;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 Index;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UInventoryItemSlot> InventorySlotBeingHovered;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* ItemName;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* ItemType;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* DamageValue;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* Durability;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* RestorationAmount;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* UsageText;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* ItemDescription;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* MaxStackSize;

	protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void GetAttributeValueFromItem(const FItemData& Item, EAttributes Attribute, int8& Value);
};
