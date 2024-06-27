

#include "InventoryTooltip.h"
#include "InventoryItemSlot.h"
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
			break;

		case EItemType::weapon:
			break;

		case EItemType::consumable:
			ItemType->SetText(FText::FromString("Consumable"));
			DamageValue->SetVisibility(ESlateVisibility::Collapsed);
			Durability->SetVisibility(ESlateVisibility::Collapsed);
			break;

		case EItemType::furniture:
			break;

		default:;
		}

		ItemName->SetText(ItemBeingHovered->TextData.name);
		DamageValue->SetText(FText::AsNumber(ItemBeingHovered->ItemStatistics.damageValue));
		Durability->SetText(FText::AsNumber(ItemBeingHovered->ItemStatistics.durability));
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
