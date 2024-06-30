

#include "InventoryTooltip.h"
#include "InventoryItemSlot.h"
#include "AVICTIMSPlayerController.h"
#include "InventoryManagerComponent.h"
#include "InventoryComponent.h"
#include "ItemDataStructs.h"
#include "ItemBase.h"
#include "Components/TextBlock.h"


void UInventoryTooltip::NativeConstruct()
{
	Super::NativeConstruct();

	if (const UItemBase* ItemBeingHovered = InventorySlotBeingHovered->GetItemReference())
	{
		switch (ItemBeingHovered->ItemQuality)
		{
		case EItemQuality::common:
			ItemType->SetColorAndOpacity(FLinearColor::Gray);
			break;
		case EItemQuality::better:
			ItemType->SetColorAndOpacity(FLinearColor::White);
			break;
		case EItemQuality::unique:
			ItemType->SetColorAndOpacity(FLinearColor(0.0f, 0.51f, 0.169f));
			break;
		default:;
		}

		switch (ItemBeingHovered->ItemType)
		{
		case EItemType::cloth:
			ItemType->SetText(FText::FromString("Cloth"));
			DamageValue->SetVisibility(ESlateVisibility::Collapsed);
			Durability->SetVisibility(ESlateVisibility::Collapsed);
			RestorationAmount->SetVisibility(ESlateVisibility::Collapsed);			
			break;

		case EItemType::weapon:
			ItemType->SetText(FText::FromString("Equipment"));
			DamageValue->SetVisibility(ESlateVisibility::Visible);
			Durability->SetVisibility(ESlateVisibility::Visible);
			RestorationAmount->SetVisibility(ESlateVisibility::Collapsed);
			break;

		case EItemType::consumable:
			ItemType->SetText(FText::FromString("Consumable"));
			DamageValue->SetVisibility(ESlateVisibility::Collapsed);
			Durability->SetVisibility(ESlateVisibility::Collapsed);
			RestorationAmount->SetVisibility(ESlateVisibility::Visible);		
			break;

		case EItemType::furniture:
			ItemType->SetText(FText::FromString("Furniture"));
			DamageValue->SetVisibility(ESlateVisibility::Collapsed);
			Durability->SetVisibility(ESlateVisibility::Collapsed);
			RestorationAmount->SetVisibility(ESlateVisibility::Collapsed);
			break;

		default:;
		}

		ItemName->SetText(ItemBeingHovered->TextData.name);
		DamageValue->SetText(FText::AsNumber(ItemBeingHovered->ItemStatistics.damageValue));
		Durability->SetText(FText::AsNumber(ItemBeingHovered->ItemStatistics.durability));
		RestorationAmount->SetText(FText::AsNumber(ItemBeingHovered->ItemStatistics.restorationAmount));
		UsageText->SetText(ItemBeingHovered->TextData.usageText);
		ItemDescription->SetText(ItemBeingHovered->TextData.description);

		if (ItemBeingHovered->NumericData.bIsStackable)
		{
			const FString StackInfo =
			{ "Max stack size: " + FString::FromInt(ItemBeingHovered->NumericData.maxStackSize) };

			MaxStackSize->SetText(FText::FromString(StackInfo));
		}
		else
		{
			MaxStackSize->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UInventoryTooltip::GetAttributeValueFromItem(const FItemData& Item, EAttributes Attribute, int8& Value)
{
	if (Attribute == EAttributes::Damage)
	{
		Value = Item.itemStatistics.damageValue;
	}
	else if (Attribute == EAttributes::Heal)
	{
		Value = Item.itemStatistics.restorationAmount;
	}
	else if (Attribute == EAttributes::Durability)
	{
		Value = Item.itemStatistics.durability;
	}
}
