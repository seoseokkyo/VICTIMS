

#include "MainMenu.h"
#include "ItemBase.h"
#include "Components/border.h"
#include "Components/Button.h"
#include "AVICTIMSPlayerController.h"
#include "InventoryPanel.h"
#include "ContainerPanel.h"
#include "HotbarPanel.h"
#include "ProfilePanel.h"
#include "ItemDragDropOperation.h"
#include "VICTIMSCharacter.h"
#include "HotbarItemSlot.h"


void UMainMenu::NativeConstruct()
{
	Super::NativeConstruct();

	PlayerCharacter = Cast<AVICTIMSCharacter>(GetOwningPlayerPawn());
	Btn_Quit->OnClicked.AddDynamic(this, &UMainMenu::OnClickedQuitButton);

	Profile = CreateWidget<UProfilePanel>(wbp_profile);
	if (Profile)
	{
		Profile->SetVisibility(ESlateVisibility::Collapsed);
	}
	Inventory = CreateWidget<UInventoryPanel>(wbp_inventory);
	if (Inventory)
	{
		Inventory->SetVisibility(ESlateVisibility::Collapsed);
	}
	Container = CreateWidget<UContainerPanel>(wbp_container);
	if (Container)
	{
		Container->SetVisibility(ESlateVisibility::Collapsed);
	}
	Hotbar = CreateWidget<UHotbarPanel>(wbp_hotbar);
	if (Hotbar)
	{
		Hotbar->SetVisibility(ESlateVisibility::Visible);
	}
	slotWidget = CreateWidget<UInventoryItemSlot>(wbp_itemSlot);
	hotbarSlot = CreateWidget<UHotbarItemSlot>(wbp_hotbarSlot);
}

void UMainMenu::OnClickedQuitButton()
{
	Profile = CreateWidget<UProfilePanel>(wbp_profile);
	if (Profile)
	{
		Profile->SetVisibility(ESlateVisibility::Collapsed);
	}
	Inventory = CreateWidget<UInventoryPanel>(wbp_inventory);
	if (Inventory)
	{
		Inventory->SetVisibility(ESlateVisibility::Collapsed);
	}
	Container = CreateWidget<UContainerPanel>(wbp_container);
	if (Container)
	{
		Container->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UMainMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

bool UMainMenu::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	const UItemDragDropOperation* ItemDragDrop = Cast<UItemDragDropOperation>(InOperation);

	if (PlayerCharacter && ItemDragDrop->SourceItem)
	{
		PlayerCharacter->DropItem(ItemDragDrop->SourceItem, ItemDragDrop->SourceItem->Quantity);
		return true;
	}
	return false;
}
