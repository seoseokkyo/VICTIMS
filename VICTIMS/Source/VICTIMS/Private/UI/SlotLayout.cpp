

#include "SlotLayout.h"
#include "DragItem.h"
#include "FWidgetsLayoutBP.h"
#include "MyHUD.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "AVICTIMSPlayerController.h"
#include "FItemQuality.h"
#include "W_ItemTooltip.h"
#include "Components/CanvasPanel.h"
#include "Internationalization/StringTableRegistry.h"
#include "VictimsGameInstance.h"
#include "Components/MenuAnchor.h"
#include "HUDLayout.h"
#include "InventoryManagerComponent.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/GameplayStatics.h>
#include <../../../../../../../Source/Runtime/Engine/Classes/Sound/SoundBase.h>

USlotLayout::USlotLayout(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void USlotLayout::NativeConstruct()
{
	Super::NativeConstruct();

	PlayerController = Cast<AVICTIMSPlayerController>(GetOwningPlayer());
	GameInstance = Cast<UVictimsGameInstance>(GetGameInstance());

	SlotMenuAnchor->SetPlacement(TEnumAsByte<EMenuPlacement>::EnumType::MenuPlacement_MenuLeft);
}

FReply USlotLayout::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		if (HasItem())
		{
			if (!SlotMenuAnchor->IsOpen())
			{
				HideTooltip();
				PlayerController->MenuAnchorIndex = InventorySlotIndex;
			}

			return FReply::Handled();
		}
	}
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		LeftMouseButtonClickedOnce = true;
		return CustomDetectDrag(InMouseEvent, this, EKeys::LeftMouseButton);
	}

	return FReply::Unhandled();
}

FReply USlotLayout::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (LeftMouseButtonClickedOnce)
	{
		UseItem();
		LeftMouseButtonClickedOnce = false;
	}
	return Super::NativeOnMouseButtonDoubleClick(InGeometry, InMouseEvent);
}

void USlotLayout::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	ItemBorder->SetBrushColor(FItemQuality::Common);
	ToggleTooltip();
}

void USlotLayout::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	LeftMouseButtonClickedOnce = false;
	ItemBorder->SetBrushColor(GetBorderColor());
	ToggleTooltip();
}

FReply USlotLayout::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return FReply::Handled();
}

void USlotLayout::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	if (HasItem())
	{
		if (DragSound)
		{
			UGameplayStatics::PlaySound2D(GetWorld(), DragSound);
		}
		HideTooltip();

		UItemDragVisual* DragVisual = CreateWidget<UItemDragVisual>(this, ItemDragVisualClass);
		DragVisual->Icon->SetBrushFromTexture(SlotStructure.ItemStructure.Icon);
		DragVisual->ItemBorder->SetBrushColor(ItemBorder->GetBrushColor());
		DragVisual->Amount->SetText(FText::AsNumber(SlotStructure.Amount));

		UDragItem* DragDropOperation = NewObject<UDragItem>();

		DragDropOperation->DefaultDragVisual = DragVisual;
		DragDropOperation->Pivot = EDragPivot::MouseDown;

		DragDropOperation->DraggedSlotInformation = SlotStructure;
		DragDropOperation->DraggedSlotIndex = InventorySlotIndex;

		if (NativeFromInventory)
		{
			DragDropOperation->IsDraggedFromInventory = true;
		}
		if (NativeFromContainer)
		{
			DragDropOperation->IsDraggedFromContainer = true;
		}
		OutOperation = DragDropOperation;
	}
	else
	{
		OutOperation = nullptr;
	}
}

bool USlotLayout::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	UDragItem* DragDropOperation = Cast<UDragItem>(InOperation);
	if (!IsValid(DragDropOperation) || DragDropOperation->DraggedSlotInformation.Amount <= 0)
	{
		return false;
	}

	const uint8 LocalDraggedSlot = DragDropOperation->DraggedSlotIndex;

	if (DragDropOperation->IsDraggedFromInventory)
	{
		if (NativeFromContainer)
		{
			if (IsUnequipping(LocalDraggedSlot))
			{
				IInventoryHUDInterface::Execute_UI_UnEquipToContainer(PlayerController, LocalDraggedSlot, InventorySlotIndex);
				return true;
			}

			IInventoryHUDInterface::Execute_UI_DepositContainerItem(PlayerController, LocalDraggedSlot, InventorySlotIndex);
			return true;
		}
		if (IsUnequipping(LocalDraggedSlot))
		{
			IInventoryHUDInterface::Execute_UI_UnEquipInventoryItem(PlayerController, LocalDraggedSlot, InventorySlotIndex);
			return true;
		}
		if (IsEquipping(InventorySlotIndex))
		{
			IInventoryHUDInterface::Execute_UI_EquipInventoryItem(PlayerController, LocalDraggedSlot, InventorySlotIndex);
			return true;
		}

		bool bSplit = false;
		if (bSplit)
		{

		}
		IInventoryHUDInterface::Execute_UI_MoveInventoryItem(PlayerController, LocalDraggedSlot, InventorySlotIndex);
		HideTooltip();

		return true;
	}

	if (DragDropOperation->IsDraggedFromContainer)
	{
		if (NativeFromInventory)
		{
			if (IsEquipping(InventorySlotIndex))
			{
				IInventoryHUDInterface::Execute_UI_EquipFromContainer(PlayerController, LocalDraggedSlot, InventorySlotIndex);
				return true;
			}

			bool bSplit = false;
			if (bSplit)
			{
				return true;
			}

			IInventoryHUDInterface::Execute_UI_TakeContainerItem(PlayerController, LocalDraggedSlot, InventorySlotIndex);
			return true;
		}

		if (NativeFromContainer)
		{
			bool bSplit = false;
			if (bSplit)
			{
				return true;
			}

			IInventoryHUDInterface::Execute_UI_MoveContainerItem(PlayerController, LocalDraggedSlot, InventorySlotIndex);
			return true;
		}
	}
	return false;
}

void USlotLayout::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("DragCancelled")));
}
void USlotLayout::ToggleTooltip()
{
	if (IsValid(GetToolTip()))
	{
		if (HasItem())
		{
			DisplayTooltip();
		}
		else
		{
			HideTooltip();
		}
	}
}

void USlotLayout::SetSlotIndex(uint8 Index)
{
	InventorySlotIndex = Index;
}

void USlotLayout::SetSlotStructure(const FSlotStructure& SlotToSet)
{
	SlotStructure = SlotToSet;
}

void USlotLayout::UpdateSlot(const FSlotStructure& NewSlotStructure)
{
	SetSlotStructure(NewSlotStructure);

	UpdateSlotInfo();

	ToggleTooltip();

	SetNameBoxVisibility();
}

void USlotLayout::UpdateSlotInfo()
{
	if (!HasItem() || (InventorySlotIndex < (uint8)EEquipmentSlot::Count && NativeFromInventory))
	{
		AmountTextBlock->SetText(FText::FromString(""));
	}
	else
	{
		AmountTextBlock->SetText(FText::AsNumber(SlotStructure.Amount));
	}

	Icon->SetBrushFromTexture(SlotStructure.ItemStructure.Icon);
	ItemBorder->SetBrushColor(GetBorderColor());

}

void USlotLayout::SetNameBoxVisibility()
{
	if (IsStorageSlot || NativeFromInventory)
	{
		NameBox->SetVisibility(ESlateVisibility::Collapsed);
		NameText->SetText(FText::GetEmpty());
	}
	else {
		FString ItemName = SlotStructure.ItemStructure.Name.ToString();
		FText ItemNameText = FText::FromString(ItemName);
		NameBox->SetVisibility(ESlateVisibility::Visible);
		NameText->SetText(ItemNameText);
	}
}

void USlotLayout::UseItem()
{
	if (NativeFromContainer)
	{
		IInventoryHUDInterface::Execute_UI_UseContainerItem(PlayerController, InventorySlotIndex);
	}
	else if (NativeFromInventory)
	{
		if (HasItem())
		{
			if (PlayerController->InventoryManagerComponent->IsSelling == false)
			{
				IInventoryHUDInterface::Execute_UI_UseInventoryItem(PlayerController, InventorySlotIndex);
			}
			else
			{
				IInventoryHUDInterface::Execute_UI_SellItem(PlayerController, InventorySlotIndex);
			}
		}
	}
	else if (NativeFromShop)
	{
		IInventoryHUDInterface::Execute_UI_PerChaseItem(PlayerController, InventorySlotIndex);
	}
}

FReply USlotLayout::CustomDetectDrag(const FPointerEvent& InMouseEvent, UWidget* WidgetDetectingDrag, FKey DragKey)
{
	if (InMouseEvent.GetEffectingButton() == DragKey || InMouseEvent.IsTouchEvent())
	{
		FEventReply Reply;
		Reply.NativeReply = FReply::Handled();

		if (WidgetDetectingDrag)
		{
			TSharedPtr<SWidget> SlateWidgetDetectingDrag = WidgetDetectingDrag->GetCachedWidget();
			if (SlateWidgetDetectingDrag.IsValid())
			{
				Reply.NativeReply = Reply.NativeReply.DetectDrag(SlateWidgetDetectingDrag.ToSharedRef(), DragKey);
				return Reply.NativeReply;
			}
		}
	}

	return FReply::Unhandled();
}

FLinearColor USlotLayout::GetBorderColor()
{
	const EItemQuality ItemQuality = SlotStructure.ItemStructure.Quality;

	switch (ItemQuality)
	{
	case EItemQuality::Common:
		return FItemQuality::Common;
	case EItemQuality::UnCommon:
		return FItemQuality::UnCommon;
	case EItemQuality::Rare:
		return FItemQuality::Rare;
	case EItemQuality::Epic:
		return FItemQuality::Epic;
	case EItemQuality::Legendary:
		return FItemQuality::Legendary;
	case EItemQuality::Undefined:
	default:
		return FItemQuality::Undefined;
	}
}

void USlotLayout::DisplayTooltip()
{
	GetToolTip()->SetVisibility(ESlateVisibility::Visible);
}

void USlotLayout::HideTooltip()
{
	if (GetToolTip())
	{
		GetToolTip()->SetVisibility(ESlateVisibility::Hidden);
	}
}

bool USlotLayout::HasItem()
{
	return SlotStructure.Amount > 0 ? true : false;
}

bool USlotLayout::IsUnequipping(const uint8& LocalDraggedSlotIndex)
{
	const uint8 NumberOfEntries = (uint8)EEquipmentSlot::Count;
	if (LocalDraggedSlotIndex < NumberOfEntries)
	{
		return true;
	}
	return false;
}

bool USlotLayout::IsEquipping(const uint8& InventorySlot)
{
	const uint8 NumberOfEntries = (uint8)EEquipmentSlot::Count;
	if (InventorySlot < NumberOfEntries)
	{
		return true;
	}
	UE_LOG(LogTemp, Warning, TEXT("Cannot equip this"))
		return false;
}
