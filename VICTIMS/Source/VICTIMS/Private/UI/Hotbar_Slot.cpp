

#include "Hotbar_Slot.h"
#include "DragItem.h"
#include "Components/Border.h"
#include "ItemDragVisual.h"
#include "FWidgetsLayoutBP.h"
#include "AVICTIMSPlayerController.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "MyHUD.h"

UHotbar_Slot::UHotbar_Slot(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	HotbarSlotIndex = 0;
	NativeFromHotbar = true;
}

void UHotbar_Slot::NativeConstruct()
{
	PlayerController = Cast<AVICTIMSPlayerController>(GetOwningPlayer());
}
void UHotbar_Slot::UseItem()
{
	IInventoryHUDInterface::Execute_UI_UseHotbarItem(PlayerController, HotbarSlotIndex);
}

void UHotbar_Slot::UpdateSlot(const FSlotStructure& NewSlotStructure)
{
	SetSlotStructure(NewSlotStructure);
	UpdateSlotInfo();
}

void UHotbar_Slot::SetSlotStructure(const FSlotStructure& SlotToSet)
{
	SlotStructure = SlotToSet;
}

void UHotbar_Slot::UpdateSlotInfo()
{
	Icon->SetBrushFromTexture(SlotStructure.ItemStructure.Icon);
}

void UHotbar_Slot::SetSlotIndex(uint8 Index)
{
	HotbarSlotIndex = Index;
}

void UHotbar_Slot::SetKeyNumber(uint8 InNumber)
{
	InNumber++;
	Number->SetText(FText::AsNumber(InNumber));
}

bool UHotbar_Slot::HasItem()
{
	return SlotStructure.Amount > 0 ? true : false;
}

FReply UHotbar_Slot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		LeftMouseButtonClickedOnce = true;
		return CustomDetectDrag(InMouseEvent, this, EKeys::LeftMouseButton);
	}
	return FReply::Unhandled();
}

FReply UHotbar_Slot::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (LeftMouseButtonClickedOnce)
	{
		UseItem();
		LeftMouseButtonClickedOnce = false;
	}

	return Super::NativeOnMouseButtonDoubleClick(InGeometry, InMouseEvent);
}

void UHotbar_Slot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	if (HasItem())
	{

		FWidgetsLayoutBP* WidgetLayout = Cast<AMyHUD>(PlayerController->HUD_Reference)->GetWidgetBPClass("ItemDragVisual_WBP");
		if (WidgetLayout)
		{
			ItemDragVisualClass = WidgetLayout->Widget;
		}

		UItemDragVisual* DragVisual = CreateWidget<UItemDragVisual>(this, ItemDragVisualClass);
		DragVisual->Icon->SetBrushFromTexture(SlotStructure.ItemStructure.Icon);
		DragVisual->ItemBorder->SetBrushColor(ItemBorder->GetBrushColor());

		UDragItem* DragDropOperation = NewObject<UDragItem>();

		DragDropOperation->DefaultDragVisual = DragVisual;
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

bool UHotbar_Slot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	UDragItem* DragDropOperation = Cast<UDragItem>(InOperation);
	if (!IsValid(DragDropOperation) || DragDropOperation->DraggedSlotInformation.Amount <= 0)
	{
		return false;
	}

	const uint8 DraggedSlotIndex = DragDropOperation->DraggedSlotIndex;
	bool bIsDraggedFromInventory = DragDropOperation->IsDraggedFromInventory;
	bool bIsDraggedFromHotbar = DragDropOperation->IsDraggedFromHotbar;

	IInventoryHUDInterface::Execute_UI_MoveHotbarItem(PlayerController, DraggedSlotIndex, HotbarSlotIndex,
		bIsDraggedFromInventory, bIsDraggedFromHotbar);

	return true;
}

FReply UHotbar_Slot::CustomDetectDrag(const FPointerEvent& InMouseEvent, UWidget* WidgetDetectingDrag, FKey DragKey)
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
