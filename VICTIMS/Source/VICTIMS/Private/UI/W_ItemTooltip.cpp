

#include "W_ItemTooltip.h"
#include "AVICTIMSPlayerController.h"
#include "InventoryComponent.h"
#include "InventoryManagerComponent.h"
#include "Internationalization/StringTableRegistry.h"
#include "FItemType.h"
#include "VictimsGameInstance.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"

void UW_ItemTooltip::NativeConstruct()
{
}

void UW_ItemTooltip::UpdateTooltipAttributes(const FItemStructure& Item, const FSlotStructure& EquippedSlot)
{
	EquippedSlotOnProfile = EquippedSlot;
	SetItemName(Item);
	Icon->SetBrushFromTexture(Item.Icon);
	SetItemType(Item);
	SetDescription(Item);
	SetAttributes(Item);
}

void UW_ItemTooltip::SetDescription(const FItemStructure& Item)
{

	Description->SetText(Item.Description);
}

void UW_ItemTooltip::SetItemType(const FItemStructure& Item)
{
	FName ItemType;
	switch (Item.ItemType)
	{
	case EItemType::Consumable:
		ItemType = FItemType::Consumable;
		Type->SetText(FText::FromString("Consumable"));
		DamageValue->SetVisibility(ESlateVisibility::Collapsed);
		HealValue->SetVisibility(ESlateVisibility::Visible);
		break;
	case EItemType::Equipment:
		ItemType = FItemType::Equipment;
		Type->SetText(FText::FromString("Equipment"));
		DamageValue->SetVisibility(ESlateVisibility::Visible);
		HealValue->SetVisibility(ESlateVisibility::Collapsed);
		break;
	case EItemType::Furniture:
		ItemType = FItemType::Furniture;
		Type->SetText(FText::FromString("Furniture"));
		DamageValue->SetVisibility(ESlateVisibility::Collapsed);
		HealValue->SetVisibility(ESlateVisibility::Collapsed);
		break;
	case EItemType::Currency:
		ItemType = FItemType::Currency;
		Type->SetText(FText::FromString("Currency"));
		DamageValue->SetVisibility(ESlateVisibility::Collapsed);
		HealValue->SetVisibility(ESlateVisibility::Collapsed);
		break;
	case EItemType::Undefined:
	default:
		ItemType = FItemType::Undefined;
		Type->SetText(FText::FromString("Undefined"));
		DamageValue->SetVisibility(ESlateVisibility::Collapsed);
		HealValue->SetVisibility(ESlateVisibility::Collapsed);
		break;
	}
}

void UW_ItemTooltip::SetItemName(const FItemStructure& Item)
{
	FText ItemNameText = Item.Name;
	Name->SetText(ItemNameText);
}

void UW_ItemTooltip::SetAttributes(const FItemStructure& Item)
{
	DamageValue->SetText(FText::AsNumber(Item.Strength));
	HealValue->SetText(FText::AsNumber(Item.Health));
	PriceValue->SetText(FText::AsNumber(Item.PriceValue));
}

void UW_ItemTooltip::GetAttributeValueFromItem(const FItemStructure& Item, EAttributes Attribute, int8& Value)
{
	if (Attribute == EAttributes::Strength)
	{
		Value = Item.Strength;
	}
	else
	{
		Value = 0;
	}
}
