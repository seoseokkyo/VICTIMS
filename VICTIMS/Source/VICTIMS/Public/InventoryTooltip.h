
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryTooltip.generated.h"

class UTextBlock;
class UItemBase;
class UInventoryItemSlot;

UCLASS()
class VICTIMS_API UInventoryTooltip : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UInventoryItemSlot> InventorySlotBeingHovered;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> ItemName;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> ItemType;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> DamageValue;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Durability;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> UsageText;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> ItemDescription;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> MaxStackSize;
	protected:
	virtual void NativeConstruct() override;
};
