

#include "InventoryItemSlot.h"
#include "ItemDragDropOperation.h"
#include "DragItemVisual.h"
#include "MainHUD.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "AVICTIMSPlayerController.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "ItemDataStructs.h"
#include "InventoryTooltip.h"
#include "Components/CanvasPanel.h"
#include "Components/MenuAnchor.h"
#include "MainMenu.h"


void UInventoryItemSlot::NativeConstruct()
{
	Super::NativeConstruct();

}

void UInventoryItemSlot::OpenSlotMenu()
{
	if (SlotStructure.amount > 0)
	{
		slotMenuAnchor->Open(true);
	}
}

void UInventoryItemSlot::CloseSlotMenu()
{
	slotMenuAnchor->Close();

}

void UInventoryItemSlot::SetSlotIndex(uint8 Index)
{
	InventorySlotIndex = Index;
}

void UInventoryItemSlot::SetSlotStructure(const FSlotStructure& SlotToSet)
{
	SlotStructure = SlotToSet;
}

void UInventoryItemSlot::UpdateSlot(const FSlotStructure& NewSlotStructure)
{
	SetSlotStructure(NewSlotStructure);
	UpdateSlotInfo();
}

void UInventoryItemSlot::DisplayTooltip()
{
	GetToolTip()->SetVisibility(ESlateVisibility::Visible);

}

void UInventoryItemSlot::HideTooltip()
{
	GetToolTip()->SetVisibility(ESlateVisibility::Hidden);

}
void UInventoryItemSlot::ToggleTooltip()
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
void UInventoryItemSlot::UpdateSlotInfo()
{
	if (!HasItem() || (InventorySlotIndex < (uint8)EEquipmentSlot::Count && NativeFromInventory))
	{
		amountTextBlock->SetText(FText::FromString(""));
	}
	else
	{
		amountTextBlock->SetText(FText::AsNumber(SlotStructure.amount));
	}

	icon->SetBrushFromTexture(SlotStructure.itemStructure.assetData.icon);
}

void UInventoryItemSlot::UseItem()
{
	if (HasItem())
	{
		IHUDInterface::Excute_UI_UseInventoryItem(PlayerController, InventorySlotIndex);
	}
	CloseSlotMenu();
}

bool UInventoryItemSlot::IsUnequipping(const uint8& LocalDraggedSlotIndex)
{
	const uint8 numberOfEntries = (uint8)EEquipmentSlot::Count;
	if (LocalDraggedSlotIndex < numberOfEntries)
	{
		return true;
	}
	return false;
}

bool UInventoryItemSlot::IsEquipping(const uint8& InventorySlot)
{
	const uint8 numberOfEntries = (uint8)EEquipmentSlot::Count;
	if (InventorySlot < numberOfEntries)
	{
		return true;
	}
	return false;
}

bool UInventoryItemSlot::HasItem()
{
	return SlotStructure.amount > 0 ? true : false;
}

FReply UInventoryItemSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		if (HasItem())
		{
			if (!slotMenuAnchor->IsOpen())
			{
				HideTooltip();
				playerController->menuAnchorIndex = InventorySlotIndex;
				OpenSlotMenu();
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

void UInventoryItemSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	LeftMouseButtonClickedOnce = false;
	ToggleTooltip();
}

FReply UInventoryItemSlot::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return FReply::Handled();
}

void UInventoryItemSlot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	if (HasItem())
	{
		HideTooltip();

		UDragItemVisual* dragVisual = CreateWidget<UDragItemVisual>(this, itemDragVisualClass);
		UItemDragDropOperation* dragDropOperation = NewObject<UItemDragDropOperation>();

		dragDropOperation->DefaultDragVisual = dragVisual;
		dragDropOperation->Pivot = EDragPivot::MouseDown;

		dragDropOperation->DraggedSlotInformation = SlotStructure;
		dragDropOperation->DraggedSlotIndex = InventorySlotIndex;

		if (NativeFromInventory)
		{
			dragDropOperation->IsDraggedFromInventory = true;
		}
		if (NativeFromContainer)
		{
			dragDropOperation->IsDraggedFromContainer = true;
		}

		OutOperation = dragDropOperation;
	}
	else
	{
		OutOperation = nullptr;
	}
}

bool UInventoryItemSlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	UItemDragDropOperation* DragDropOperation = Cast<UItemDragDropOperation>(InOperation);
	if (!IsValid(DragDropOperation) || DragDropOperation->DraggedSlotInformaion.amount <= 0)
	{
		return false;
	}
	const uint8 LocalDraggedSlot = DragDropOperation->DraggedSlotIndex;

	if (DragDropOperation->IsDraggedfromInventory)
	{
		if (NativeFromContainer)
		{
			if (IsUnequipping(LocalDraggedSlot))
			{
				IHUDInterface::Execute_UI_UnEquipToContainer(PlayerController, LocalDraggedSlot, InventorySlotIndex);
				return true;
			}
			IHUDInterface::Execute_UI_DepositContainerItem(PlayerController, LocalDraggedSlot, InventorySlotIndex);
			return true;
		}

		if (IsUnequipping(LocalDraggedSlot))
		{
			IHUDInterface::Execute_UI_UnEquipInventoryItem(PlayerController, LocalDraggedSlot, InventorySlotIndex);
			return true;
		}
		if (IsEquipping(LocalDraggedSlot))
		{
			IHUDInterface::Execute_UI_EquipInventoryItem(PlayerController, LocalDraggedSlot, InventorySlotIndex);
			return true;
		}

		bool bSplit = false;
		if (bSplit)
		{
		}
		IHUDInterface::Execute_UI_MoveInventoryItem(PlayerController, LocalDraggedSlot, InventorySlotIndex);
		HideTooltip();

		return true;
	}
	if (DragDropOperation->IsDraggedFromContainer)
	{
		if (NativeFromInventory)
		{
			bool bSplit = false;
			if (bSplit)
			{
				return true;
			}
			IHUDInterface::Execute_UI_TakeContainerItem(PlayerController, LocalDraggedSlot, InventorySlotIndex);
			CloseSlotMenu();
			return true;
		}
	}
	return false;
}

void UInventoryItemSlot::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);
}

FReply UInventoryItemSlot::CustomDetectDrag(const FPointerEvent& InMouseEvent, UWidget* WidgetDetectingDrag, FKey DragKey)
{
	if (InMouseEvent.GetEffectingButton() == DragKey)
	{
		FEventReply reply;
		reply.NativeReply = FReply::Handled();

		if (WidgetDetectingDrag)
		{
			TSharedPtr<SWidget> slateWidgetDetectingDrag = WidgetDetectingDrag->GetCachedWidget();
			if (slateWidgetDetectingDrag.IsValid())
			{
				reply.NativeReply = reply.NativeReply.DetectDrag(slateWidgetDetectingDrag.ToSharedRef(), DragKey);
				return reply.NativeReply;
			}
		}
	}
	return FReply::Unhandled();
}

FReply UInventoryItemSlot::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (LeftMouseButtonClickedOnce)
	{
		UseItem();
		LeftMouseButtonClickedOnce = false;
	}
	return Super::NativeOnMouseButtonDoubleClick(InGeometry, InMouseEvent);
}

void UInventoryItemSlot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	ToggleeTooltip();
}
