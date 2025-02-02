


#include "MainLayout.h"
#include "DragItem.h"
#include "DragWidget.h"
#include "AVICTIMSPlayerController.h"
#include "Attribute_Entry.h"
#include "InventoryLayout.h"
#include "Components/UniformGridpanel.h"
#include "ProfileLayout.h"
#include "InteractText.h"
#include "ShopLayout.h"
#include "DropMoneyLayout.h"
#include "ItemDragVisual.h"
#include "SavedWidget.h"

void UMainLayout::NativeConstruct()
{
	Super::NativeConstruct();  

	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	DropMoneyLayout->SetVisibility(ESlateVisibility::Hidden);
	Saved->SetVisibility(ESlateVisibility::Hidden);
}

bool UMainLayout::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	UDragWidget* DragWidgetResult = Cast<UDragWidget>(InOperation);
	if (!IsValid(DragWidgetResult))
	{
		// Drop Dragged Inventory Item
		UDragItem* DragItemResult = Cast<UDragItem>(InOperation);
		if (!IsValid(DragItemResult))
		{
			return false;
		}

		if (DragItemResult->IsDraggedFromInventory)
		{
			if (AVICTIMSPlayerController* PlayerController = Cast<AVICTIMSPlayerController>(GetOwningPlayer()))
			{
				IInventoryHUDInterface::Execute_UI_DropInventoryItem(PlayerController, DragItemResult->DraggedSlotIndex);
				return true;
			}
		}

		return false;
	} //Drop Dragged UI Window
	else
	{
		const FVector2D DragWindowOffset = InGeometry.AbsoluteToLocal(InDragDropEvent.GetScreenSpacePosition());
		const FVector2D DragWindowOffsetResult = DragWindowOffset - DragWidgetResult->DragOffset;

		DragWidgetResult->WidgetReference->AddToViewport();
		DragWidgetResult->WidgetReference->SetVisibility(ESlateVisibility::Visible);
		DragWidgetResult->WidgetReference->SetPositionInViewport(DragWindowOffsetResult, false);

		if (Inventory->GetVisibility() == ESlateVisibility::HitTestInvisible)
		{
			Inventory->SetVisibility(ESlateVisibility::Visible);

		}
		if (Profile->GetVisibility() == ESlateVisibility::HitTestInvisible)
		{
			Profile->SetVisibility(ESlateVisibility::Visible);
		}
		if (Shop->GetVisibility() == ESlateVisibility::HitTestInvisible)
		{
			Shop->SetVisibility(ESlateVisibility::Visible);
		}

		return true;
	}
}
