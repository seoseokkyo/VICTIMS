

#include "HotbarItemSlot.h"
#include "ItemDragDropOperation.h"
#include "DragItemVisual.h"
#include "AVICTIMSPlayerController.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "MainHUD.h"

void UHotbarItemSlot::NativeConstruct()
{
	PlayerController = Cast<AVICTIMSPlayerController>(GetOwningPlayer());

}

void UHotbarItemSlot::UseItem()
{
	IHUDInterface::Execute_UI_UseHotbarItem(PlayerController, HotbarSlotIndex);
}

void UHotbarItemSlot::UpdateSlot(const FSlotStructure& NewSlotStructure)
{
	SetSlotStructure(NewSlotStructure);
	UpdateSlotInfo();
}

void UHotbarItemSlot::SetSlotStructure(const FSlotStructure& SlotToSet)
{
	SlotStructure = SlotToSet;
}

void UHotbarItemSlot::UpdateSlotInfo()
{
	Icon->SetBrushFromTexture(SlotStructure.itemStructure.assetData.icon);
}

void UHotbarItemSlot::SetSlotIndex(uint8 Index)
{
	HotbarSlotIndex = Index;
}

void UHotbarItemSlot::SetKeyNumber(uint8 InNumber)
{
	InNumber++;
	Number->SetText(FText::AsNumber(InNumber));
}

bool UHotbarItemSlot::HasItem()
{
	return SlotStructure.amount > 0 ? true : false;
}

FReply UHotbarItemSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		LeftMouseButtonClickedOnce = true;
		return CustomDetectDrag(InMouseEvent, this, EKeys::LeftMouseButton);
	}
	return FReply::Unhandled();
}

FReply UHotbarItemSlot::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (LeftMouseButtonClickedOnce)
	{
		UseItem();
		LeftMouseButtonClickedOnce = false;
	}
	return Super::NativeOnMouseButtonDoubleClick(InGeometry, InMouseEvent);
}

void UHotbarItemSlot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	if (HasItem())	
	{
		UDragItemVisual* DragVisual = CreateWidget<UDragItemVisual>(this, ItemDragVisualClass);
		DragVisual->ItemIcon->SetBrushFromTexture(SlotStructure.itemStructure.assetData.icon);
		UDragDropOperation* DragDropOperation = NewObject<UDragDropOperation>();
		DragDropOperation->Pivot = EDragPivot::MouseDown;

		DragDropOperation->DraggedSlotInformation = SlotStructure;
		DragDropOperation->DraggedSlotIndex = HotbarSlotIndex;

		DragDropOperation->IsDraggedFromHotbar = true;

		OutOperation = DragDropOperation;
	}
	else
	{
		OutOperation = nullptr;
	}
}

bool UHotbarItemSlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
	UItemDragDropOperation* DragDropOperation = Cast<UItemDragDropOperation>(InOperation);
	if (!IsValid(DragDropOperation) || DragDropOperation-> < DraggedSlotInformation.Amout <= 0)
	{
		return false;
	}

	const uint8 DraggedSlotIndex = DragDropOperation->DraggedSlotIndex;
	bool bIsDraggedFromInventory = DragDropOperation->IsDraggedFromInventory;
	bool bIsDraggedFromHotbar = DragDropOperation->IsDraggedFromHotbar;

	IHUDInterface::Execute_UI_MoveHotbarItem(PlayerController, DraggedSlotIndex, HotbarSlotIndex, bIsDraggedFromInventory, bIsDraggedFromHotbar);
	return true;

}

FReply UHotbarItemSlot::CustomDetectDrag(const FPointerEvent& InMouseEvent, UWidget* WidgetDetectingDrag, FKey DragKey)
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
