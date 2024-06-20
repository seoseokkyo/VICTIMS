
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryToolTip.generated.h"


class UTextBlock;
//class UInventoryItemSlot;

UCLASS()
class VICTIMS_API UInventoryToolTip : public UUserWidget
{
	GENERATED_BODY()

public:
	
//	UPROPERTY(VisibleAnywhere)
//	UInventoryItemSlot* inventorySlotClicked;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* itemName;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* itemType;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* itemPrice;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* itemDurability;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* itemDescription;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* maxStackCount;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* currentStackCount;

protected:
	virtual void NativeConstruct() override;
	
};
