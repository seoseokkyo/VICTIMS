

#include "InventoryManagerComponent.h"
#include "EAttributes.h"
#include "MainHUD.h"
#include "PickUp.h"
#include "EquipmentComponent.h"
#include "InventoryComponent.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "MainMenu.h"
#include "InventoryPanel.h"
#include "ContainerPanel.h"
#include "HotbarPanel.h"
#include "ProfilePanel.h"
#include "InventoryTooltip.h"
#include "InventoryItemSlot.h"
#include "GameFramework/PlayerState.h"
#include "FContainerInfo.h"
#include "Net/UnrealNetwork.h"
#include "AVICTIMSPlayerController.h"
#include "HotbarItemSlot.h"

UInventoryManagerComponent::UInventoryManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}
void UInventoryManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	Gold = 0;
	totalNumberOfSlots = (NumberOfRowsInventory * SlotsPerRowInventory) + (uint8)EEquipmentSlot::Count;

	InitializeItemDB();
}
void UInventoryManagerComponent::InitializeItemDB()
{
	UDataTable* bp_ItemDB = LoadObject<UDataTable>(this, TEXT("/ Script / Engine.DataTable'/Game/Item/Data/BP_ItemData.BP_ItemData'"));
	if (IsValid(bp_ItemDB))
	{
		itemDB = bp_ItemDB;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemDB not Loaded"))

	}
}

void UInventoryManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UInventoryManagerComponent, totalNumberOfSlots);
	DOREPLIFETIME(UInventoryManagerComponent, AttributesArray);
	DOREPLIFETIME(UInventoryManagerComponent, Gold);
}
void UInventoryManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UInventoryManagerComponent::InitializeInventoryManager(UInventoryComponent* EquipmentComponent)
{
	playerInventory = EquipmentComponent;
}

void UInventoryManagerComponent::Server_InitInventory_Implementation()
{
	playerInventory->Server_InitInventory(totalNumberOfSlots);
}
void UInventoryManagerComponent::OnRep_UpdateGoldAmount()
{
	if (IsValid(mainLayoutUI))
	{
		mainLayoutUI->Inventory->UpdateGoldAmount();
	}
}

void UInventoryManagerComponent::Client_SetAttributes(const TArray<uint8>& InAttributesArray)
{
	SetAttributes(InAttributesArray);
}

void UInventoryManagerComponent::SetAttributes(const TArray<uint8>& InAttributesArray)
{
	if (IsValid(mainLayoutUI))
	{
		mainLayoutUI->Profile->UpdatePlayerStatsUI(InAttributesArray);
	}
}

void UInventoryManagerComponent::TryToAddItemToInventory(UInventoryComponent* Inventory, FSlotStructure& InventoryItem, bool& bOutSuccess)
{
	FName localItemID = InventoryItem.itemStructure.ID;
	uint8 localItemAmount = InventoryItem.amount;
	FSlotStructure localInventoryItem = InventoryItem;
	UInventoryComponent* localInventory = Inventory;

	uint8 amountRemaing = localItemAmount;

	if (localInventoryItem.itemStructure.itemType == EItemType::currency)
	{
		AddGold(localInventoryItem.amount);
		bOutSuccess = true;
		return;
	}
	if (localInventoryItem.itemStructure.numericData.bIsStackable)
	{
		FindAndAddAmountToStacks(localInventory, localItemID, localItemAmount, amountRemaing);
		localItemAmount = amountRemaing;
		if (localItemAmount == 0)
		{
			bOutSuccess = true;
			return;
		}
		else
		{
			localInventoryItem.amount = localItemAmount;
		}

		FReturnTupleBoolInt localTuple;
		if (localInventory == playerInventory)
		{
			localTuple = localInventory->GetEmptyInventorySpace();
		}
		else if (localInventory == containerInventory)
		{
			localTuple = localInventory->GetEmptyContainerSpace();
		}

		if (localTuple.Success)
		{
			AddItem(localInventory, localTuple.Index, localInventoryItem);
			bOutSuccess = true;
			return;
		}
		else
		{
			UE_LOG(LogTemp, Verbose, TEXT("Inventory Full"))
			bOutSuccess = false;
			return;
		}
	}
}
void UInventoryManagerComponent::Server_UseInventoryItem_Implementation(const uint8& InventorySlot)
{
	UseInventoryItem(InventorySlot);
}
void UInventoryManagerComponent::Server_UseContainerItem_Implementation(const uint8& InventorySlot)
{
	UseContainer(CurrentContainer);
}
void UInventoryManagerComponent::Server_MoveInventoryItem_Implementation(uint8 FromInventorySlot, uint8 ToInventorySlot)
{
	MoveItem(playerInventory, FromInventorySlot, playerInventory, ToInventorySlot);
}
void UInventoryManagerComponent::Server_EquipFromInventory_Implementation(uint8 FromInventorySlot, uint8 ToInventorySlot)
{
	EquipItem(playerInventory, FromInventorySlot, playerInventory, ToInventorySlot);
}
void UInventoryManagerComponent::Server_UnEquipToInventory_Implementation(uint8 FromInventorySlot, uint8 ToInventorySlot)
{
	UnEquipItem(playerInventory, FromInventorySlot, playerInventory, ToInventorySlot);
}
void UInventoryManagerComponent::Server_DropItemFromInventory_Implementation(const uint8& InventorySlot)
{
	DropItem(playerInventory, InventorySlot);
}
void UInventoryManagerComponent::Server_UseContainer_Implementation(AActor* Container)
{
	UseContainer(Container);
}
void UInventoryManagerComponent::Server_CloseContainer_Implementation()
{
	CloseContainer();
}

void UInventoryManagerComponent::Server_EquipFromContainer_Implementation(uint8 FromInventorySlot, uint8 ToInventorySlot)
{
	EquipItem(containerInventory, FromInventorySlot, playerInventory, ToInventorySlot);
}

void UInventoryManagerComponent::Server_UnEquipToContainer_Implementation(uint8 FromInventorySlot, uint8 ToInventorySlot)
{
	UnEquipItem(playerInventory, FromInventorySlot, containerInventory, ToInventorySlot);
}

void UInventoryManagerComponent::Client_OpenContainer_Implementation(FContainerInfo ContainerProperties, const TArray<FSlotStructure>& InContainerInventory, const TArray<FSlotStructure>& InPlayerInventory)
{
	LoadContainerSlots(ContainerProperties, InContainerInventory, InPlayerInventory);
}
void UInventoryManagerComponent::Client_CloseContainer_Implementation()
{
	mainLayoutUI->Container->SetVisibility(ESlateVisibility::Hidden);
}
void UInventoryManagerComponent::Server_TakeContainerItem_Implementation(const uint8& FromContainerSlot, const uint8& ToInventorySlot)
{
	MoveItem(containerInventory, FromContainerSlot,playerInventory, ToInventorySlot);
}
void UInventoryManagerComponent::Server_DepositContainerItem_Implementation(const uint8& FromInventorySlot, const uint8& ToInventorySlot)
{
	MoveItem(playerInventory, FromInventorySlot, containerInventory, ToInventorySlot);
}
void UInventoryManagerComponent::Server_MoveContainerItem_Implementation(uint8 FromInventorySlot, uint8 ToInventorySlot)
{
	MoveItem(containerInventory, FromInventorySlot, containerInventory, ToInventorySlot);
}
void UInventoryManagerComponent::Client_SetInventorySlotItem_Implementation(const FSlotStructure& ContentToAdd, const uint8& InventorySlot)
{
	SetInventorySlotItem(ContentToAdd, InventorySlot);
}
void UInventoryManagerComponent::Client_SetContainerSlotItem_Implementation(const FSlotStructure& ContentToAdd, const uint8& InventorySlot)
{
	SetContainerSlotItem(ContentToAdd, InventorySlot);
}
void UInventoryManagerComponent::Client_ClearInventorySlotItem_Implementation(uint8 InventorySlot)
{
	ClearInventorySlotItem(InventorySlot);
}
void UInventoryManagerComponent::Client_ClearContainerSlotItem_Implementation(uint8 ContainerSlot)
{
	ClearContainerSlotItem(ContainerSlot);
}
void UInventoryManagerComponent::Client_ClearHotbarSlot_Implementation(const uint8& HotbarSlot)
{
	ClearHotbarSlotItem(HotbarSlot);
}
void UInventoryManagerComponent::Client_MoveHotbarSlotItem_Implementation(const uint8& FromSlot, const uint8& ToSlot, const bool IsDraggedFromInventory, const bool IsDraggedFromHotbar)
{
	MoveHotbarSlotItem(FromSlot, ToSlot, IsDraggedFromInventory, IsDraggedFromHotbar);
}
void UInventoryManagerComponent::Client_UseHotbarSlot_Implementation(const uint8& HotbarSlot)
{
	UseHotbarSlot(HotbarSlot);
}
void UInventoryManagerComponent::Client_LoadInventoryUI_Implementation()
{
	AVICTIMSPlayerController* pc = Cast<AVICTIMSPlayerController>(GetOwner());

	AMainHUD* widgetLayout = Cast<AMainHUD>(pc->HUD_Reference);
	if (widgetLayout)
	{
		UInventoryItemSlot* w_Slot = nullptr;
																			// ¿Œ∫•≈‰∏Æ ΩΩ∑‘¿ß¡¨ √§øÏ±‚
		for (int i = 0; i < NumberOfRowsInventory; i++)
		{
			for (int j = 0; j < SlotsPerRowInventory; j++)
			{
				w_Slot = CreateWidget<UInventoryItemSlot>(GetWorld(), widgetLayout->MainMenuWidget->slot);
				mainLayoutUI->Inventory->InventoryGridPanel->AddChildToUniformGrid(w_Slot, i, j);
				mainLayoutUI->Inventory->inventorySlotsArray.Add(w_Slot);
			}
		}

		FSlotStructure slotStructure = GetEmptySlot(EEquipmentSlot::Undefined);			// ¿Œ∫•≈‰∏Æ ΩΩ∑‘ º≥¡§ 

		UInventoryItemSlot* localSlot{};
		for (int i = 0; i < mainLayoutUI->Inventory->inventorySlotsArray.Num(); i++)
		{
			localSlot = mainLayoutUI->Inventory->inventorySlotsArray[i];
			localSlot->SetSlotIndex(i + (uint8)EEquipmentSlot::Count);
			localSlot->NativeFromInventory = true;
			localSlot->UpdateSlot(slotStructure);
		}
	}
}
void UInventoryManagerComponent::Client_LoadProfileUI_Implementation()
{
	AVICTIMSPlayerController* pc = Cast<AVICTIMSPlayerController>(GetOwner());
	AMainHUD* widgetLayout = Cast<AMainHUD>(pc->HUD_Reference);
	if (widgetLayout)
	{
		UInventoryItemSlot* w_Slot = nullptr;											// ¿Â∫Ò ΩΩ∑‘ 
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				w_Slot = CreateWidget<UInventoryItemSlot>(GetWorld(), widgetLayout->MainMenuWidget->slot);
				UUniformGridSlot* gridSlot = mainLayoutUI->Profile->EquipmentGridpanel->AddChildToUniformGrid(w_Slot, i, j);
				gridSlot->SetHorizontalAlignment(HAlign_Center);
				gridSlot->SetVerticalAlignment(VAlign_Center);
				mainLayoutUI->Profile->EquipmentSlotArray.Add(w_Slot);
			}
		}
		FSlotStructure slotStructure{};
		UInventoryItemSlot* localSlot{};

		for (int i = 0; i < mainLayoutUI->Profile->EquipmentSlotArray.Num(); i++)
		{
			if (i == 0)
			{
				slotStructure = GetEmptySlot(EEquipmentSlot::Weapon);
			}
			else if (i == 1)
			{
				slotStructure = GetEmptySlot(EEquipmentSlot::Head);
			}
			else if (i == 2)
			{
				slotStructure = GetEmptySlot(EEquipmentSlot::Top);
			}
			else if (i == 3)
			{
				slotStructure = GetEmptySlot(EEquipmentSlot::Bottom);
			}
			else if (i == 4)
			{
				slotStructure = GetEmptySlot(EEquipmentSlot::Feet);
			}
			else
			{
				slotStructure = GetEmptySlot(EEquipmentSlot::Undefined);
			}

			localSlot = mainLayoutUI->Profile->EquipmentSlotArray[i];
			localSlot->SetSlotIndex(i);
			localSlot->NativeFromInventory = true;
			localSlot->UpdateSlot(slotStructure);
		}
	}

}
void UInventoryManagerComponent::Client_LoadHotbarUI_Implementation()
{
	LoadHotbarUI();
}
void UInventoryManagerComponent::LoadHotbarUI()
{
	AVICTIMSPlayerController* pc = Cast<AVICTIMSPlayerController>(GetOwner());		// ƒ¸ΩΩ∑‘ ¿Œ∫• ΩΩ∑‘
	AMainHUD* widgetLayout = Cast<AMainHUD>(pc->HUD_Reference);
	if (widgetLayout)
	{
		UHotbarItemSlot* w_slot = nullptr;
		uint8 row = 0;
		uint8 column = 0;

		uint8 keyNumber = 0;
		
		for (uint8 i = 0; i < NumberOfSlotsOnHotbar; i++)
		{
			column = i;

			w_slot = CreateWidget<UHotbarItemSlot>(GetWorld(), widgetLayout->MainMenuWidget->hotbarSlot);
			keyNumber = i;

			if (keyNumber == pc->GetMaximumHotbarSlots())
			{
				keyNumber = pc->GetMaximumHotbarSlots();
			}

			w_slot->SetKeyNumber(keyNumber);

			mainLayoutUI->Hotbar->HotbarGridPanel->AddChildToUniformGrid(w_slot, row, column);
			mainLayoutUI->Hotbar->HotbarSlotsArray.Add(w_slot);
		}
		
		FSlotStructure slotStructure = GetEmptySlot(EEquipmentSlot::Undefined);
		UHotbarItemSlot* localSlot{};
		for (int i = 0; i < mainLayoutUI->Hotbar->HotbarSlotsArray.Num(); i++)
		{
			localSlot = mainLayoutUI->Hotbar->HotbarSlotsArray[i];
			localSlot->SetSlotIndex(i);
			localSlot->NativeFromHotbar = true;
			localSlot->UpdateSlot(slotStructure);
		}
	}

}
void UInventoryManagerComponent::Client_CheckHotbarSlots_Implementation(const FSlotStructure& Slot)
{
	bool Success = false;
	TArray<UInventoryItemSlot*> localInventoryUI2 = mainLayoutUI->Inventory->inventorySlotsArray;

	for (uint8 i = 0; i < localInventoryUI2.Num(); i++)
	{
		if (Slot.itemStructure.ID == localInventoryUI2[i]->SlotStructure.itemStructure.ID)
		{
			Success = true;
			break;
		}
	}
	if (!Success)
	{
		TArray<UHotbarItemSlot*> hotbar = mainLayoutUI->Hotbar->HotbarSlotsArray;
		for (uint8 i = 0; i < hotbar.Num(); i++)
		{
			if (Slot.itemStructure.ID == GetHotbarSlotItem(i).itemStructure.ID)
			{
				ClearHotbarSlotItem(i);
				break;
			}
		}
	}
}
void UInventoryManagerComponent::InitializeInventoryManagerUI(UMainMenu* MainLayout)
{
	mainLayoutUI = MainLayout;
}
EEquipmentSlot UInventoryManagerComponent::GetEquipmentTypeBySlot(const uint8& EquipmentSlot)
{
	switch (EquipmentSlot)
	{
	case 0: return EEquipmentSlot::Weapon;
	case 1: return EEquipmentSlot::Head;
	case 2: return EEquipmentSlot::Top;
	case 3: return EEquipmentSlot::Bottom;
	case 4: return EEquipmentSlot::Feet;
	default: return EEquipmentSlot::Undefined;
	}
}
EItemType UInventoryManagerComponent::GetItemTypeBySlot(uint8 ItemSlot)
{
	return playerInventory->Inventory[ItemSlot].itemStructure.itemType;
}
void UInventoryManagerComponent::AddItem(UInventoryComponent* Inventory, uint8 InventorySlot, FSlotStructure& InventoryItem)
{
	Inventory->SetInventoryItem(InventorySlot, InventoryItem);
	if (playerInventory == Inventory)
	{
		Client_SetInventorySlotItem(InventoryItem, InventorySlot);
	}
	else
	{
		SetViewersContainerSlot(InventorySlot, InventoryItem);
	}
}
FSlotStructure UInventoryManagerComponent::GetEmptySlot(EEquipmentSlot FromEquipmentType)
{
	FName name;
	if (FromEquipmentType == EEquipmentSlot::Weapon)
	{
		name = "No_Weapon";
	}
	else if (FromEquipmentType == EEquipmentSlot::Head)
	{
		name = "No_Helmet";
	}
	else if (FromEquipmentType == EEquipmentSlot::Top)
	{
		name = "No_Top";
	}
	else if (FromEquipmentType == EEquipmentSlot::Bottom)
	{
		name = "No_Bottom";
	}
	else if (FromEquipmentType == EEquipmentSlot::Feet)
	{
		name = "No_Feet";
	}
	else
	{
		name = "Empty";
	}
	return GetItemFromItemDB(name);
}
FSlotStructure UInventoryManagerComponent::GetItemFromItemDB(FName Name)
{
	FSlotStructure slot = {};
	if (IsValid(itemDB))
	{
		const UDataTable* itemTable = itemDB;
		const FItemData* newItemData = itemTable->FindRow<FItemData>(Name, "", true);
		slot.InitSlot(*newItemData, 0);
	}
	else
	{
		InitializeItemDB();
		if (IsValid(itemDB))
		{
			const UDataTable* itemTable = itemDB;
			const FItemData* newItemData = itemTable->FindRow<FItemData>(Name, "", true);
			slot.InitSlot(*newItemData, 0);
		}
	}
	return slot;
}
void UInventoryManagerComponent::UpdateEquippedStats()
{
	uint8 tempStrength = 0;
	for (int i = 0; i < (uint8)EEquipmentSlot::Count; i++)
	{
		FSlotStructure tempslot = playerInventory->GetInventoryItem();
		tempStrength += tempslot.itemStructure.itemStatistics.damageValue;
	}
	AttributesArray[0] = tempStrength;
	Client_SetAttributes(AttributesArray);
}
UDataTable* UInventoryManagerComponent::GetItemDB()
{
	return nullptr;
}

void UInventoryManagerComponent::MoveHotbarSlotItem(const uint8& FromSlot, const uint8& ToSlot, const bool IsDraggedFromInventory, const bool IsDraggedFromHotbar)
{
	if (IsDraggedFromInventory)
	{
		FSlotStructure slotStructure = GetInventorySlotItem(FromSlot);
		SetHotbarSlotItem(ToSlot, slotStructure);
	}
	if (IsDraggedFromHotbar)
	{
		FSlotStructure fromSlotItem = GetHotbarSlotItem(FromSlot);
		FSlotStructure toSlotItem = GetHotbarSlotItem(ToSlot);
		if (ItemIsValid(toSlotItem))
		{
			SetHotbarSlotItem(ToSlot, fromSlotItem);
			SetHotbarSlotItem(FromSlot, toSlotItem);
		}
		else
		{
			SetHotbarSlotItem(ToSlot, fromSlotItem);
			ClearHotbarSlotItem(FromSlot);
		}
	}
}

void UInventoryManagerComponent::UseHotbarSlot(const uint8& HotbarSlot)
{
	FReturnTupleBoolInt tuple{};
	FSlotStructure slot = GetHotbarSlotItem(HotbarSlot);
	if (ItemIsValid(slot))
	{
		TArray<UInventoryItemSlot*> localInventoryUI = mainLayoutUI->Inventory->inventorySlotsArray;
		for (uint8 i = 0; i < localInventoryUI.Num(); i++)
		{
			if (slot.itemStructure.ID == localInventoryUI[i]->SlotStructure.itemStructure.ID)
			{
				tuple.Success = true;
				tuple.Index = i;
				break;
			}
		}
		if (tuple.Success)
		{
			tuple.Index = tuple.Index + (uint8)EEquipmentSlot::Count;
			Server_UseInventoryItem(tuple.Index);
		}
	}
}

void UInventoryManagerComponent::ClearHotbarSlotItem(const uint8& HotbarSlot)
{
	UHotbarItemSlot* slot = mainLayoutUI->Hotbar->HotbarSlotsArray[HotbarSlot];
	FSlotStructure localSlot = GetEmptySlot(EEquipmentSlot::Undefined);
	slot->UpdateSlot(localSlot);
}

void UInventoryManagerComponent::SetHotbarSlotItem(const uint8& ToSlot, FSlotStructure SlotStructure)
{
	UHotbarItemSlot* hotbarSlot = mainLayoutUI->Hotbar->HotbarSlotsArray[ToSlot];
	hotbarSlot->UpdateSlot(SlotStructure);
}

FSlotStructure UInventoryManagerComponent::GetHotbarSlotItem(const uint8& HotbarSlot)
{
	TArray<UHotbarItemSlot*> hotbarSlotsArray = mainLayoutUI->Hotbar->HotbarSlotsArray;
	FSlotStructure slot = hotbarSlotsArray[HotbarSlot]->SlotStructure;
	return slot;
}

uint8 UInventoryManagerComponent::GetEquipmentSlotByType(EEquipmentSlot EquipmentSlot)
{
	for (uint8 index = 0; index < (uint8)EEquipmentSlot::Count; index++)
	{
		EEquipmentSlot localSlotType = GetEquipmentTypeBySlot(index);
		if (EquipmentSlot == localSlotType)
		{
			return index;
		}
	}
	return 0;
}

EEquipmentSlot UInventoryManagerComponent::GetItemEquipmentSlot(FSlotStructure Item)
{
	return Item.itemStructure.equipmentSlot;
}

bool UInventoryManagerComponent::ItemIsValid(FSlotStructure Item)
{
	if (Item.amount > 0)
	{
		return true;
	}
	return false;
}

bool UInventoryManagerComponent::GetEmptyEquipmentSlotByType(EEquipmentSlot EquipmentSlot, uint8& OutIndex)
{
	for (uint8 index = 0; index < (uint8)EEquipmentSlot::Count; index++)
	{
		EEquipmentSlot localEquipSlot = GetEquipmentTypeBySlot(index);
		FSlotStructure localSlot = playerInventory->GetInventorySlot(index);
		if (!ItemIsValid(localSlot) && EquipmentSlot == localEquipSlot)
		{
			OutIndex = index;
			return true;
		}
	}
	return false;
}

bool UInventoryManagerComponent::ItemIsSame(const FSlotStructure Item1, const FSlotStructure Item2)
{
	if (Item1.itemStructure.ID == Item2.itemStructure.ID)
	{
		return true;
	}
	return false;
}

bool UInventoryManagerComponent::IsItemStackable(const FSlotStructure Item)
{
	return Item.itemStructure.numericData.bIsStackable;
}

uint8 UInventoryManagerComponent::ItemFreeStackSpace(const FSlotStructure Item)
{
	return GetItemMaxStackSize(Item) - GetItemAmount(Item);
}

uint8 UInventoryManagerComponent::GetItemAmount(const FSlotStructure Item)
{
	return Item.amount;
}

uint8 UInventoryManagerComponent::GetItemMaxStackSize(const FSlotStructure Item)
{
	return Item.itemStructure.numericData.maxStackSize;
}

void UInventoryManagerComponent::AddAmountToItem(FSlotStructure& Item, uint8 AmountToAdd)
{
	Item.amount += AmountToAdd;
}

void UInventoryManagerComponent::RemoveFromItemAmount(FSlotStructure& InventoryItem, const uint8& AmountToRemove, bool& WasFullAmountRemoved, uint8& AmountRemoved)
{
	if (AmountToRemove >= InventoryItem.amount)
	{
		AmountRemoved = InventoryItem.amount;
		WasFullAmountRemoved = true;
	}
	else
	{
		const uint8 updatedAmount = InventoryItem.amount - AmountToRemove;
		InventoryItem.amount = updatedAmount;
		WasFullAmountRemoved = false;
	}
}

void UInventoryManagerComponent::EquipItem(UInventoryComponent* FromInventory, uint8 FromInventorySlot, UInventoryComponent* ToInventory, uint8 ToInventorySlot)
{
	if (FromInventory == ToInventory && FromInventorySlot == ToInventorySlot)
	{
		return;
	}
	if (GetItemTypeBySlot(FromInventorySlot) == EItemType::cloth || GetItemTypeBySlot(FromInventorySlot) == EItemType::weapon)
	{
		FSlotStructure localInventoryItem = FromInventory->GetInventorySlot(FromInventorySlot);
		EEquipmentSlot localEquipmentSlotType = GetItemEquipmentSlot(localInventoryItem);
		if (GetEquipmentTypeBySlot(ToInventorySlot) == localEquipmentSlotType)
		{
			FSlotStructure localSwapInventoryItem = ToInventory->GetInventorySlot(ToInventorySlot);
			if (ItemIsValid(localSwapInventoryItem))
			{
				if (!CanContainerStoreItems(FromInventory))
				{
					UE_LOG(LogTemp, Warning, TEXT("CONTAINER CANNOT STORE ITEMS"))
					return;
				}
				AddItem(ToInventory, ToInventorySlot, localInventoryItem);
				AddItem(FromInventory, FromInventorySlot, localSwapInventoryItem);
			}
			else
			{
				AddItem(ToInventory, ToInventorySlot, localInventoryItem);
				RemoveItem(FromInventory, FromInventorySlot);
			}
			UpdateEquippedStats();
			return;
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("ITEM CAN NOT BE EQUIPPED IN THAT SLOT"))
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ITEM IS NOT EQUIPPABLE"))
	}
}

void UInventoryManagerComponent::UnEquipItem(UInventoryComponent* FromInventory, uint8 FromInventorySlot, UInventoryComponent* ToInventory, uint8 ToInventorySlot)
{
	if (FromInventory == ToInventory && FromInventorySlot == ToInventorySlot)
	{
		return;
	}
	FSlotStructure localInventoryItem = FromInventory->GetInventorySlot(FromInventorySlot);
	FSlotStructure localSwapInventoryItem = ToInventory->GetInventorySlot(ToInventorySlot);
	EEquipmentSlot localEquipmentSlot = GetEquipmentTypeBySlot(FromInventorySlot);
	if (ItemIsValid(localSwapInventoryItem))
	{
		if (!CanContainerStoreItems(ToInventory))
		{
			UE_LOG(LogTemp, Warning, TEXT("CONTAINER CANNOT STORE ITEMS"))
				return;
		}

		if (GetItemTypeBySlot(FromInventorySlot) != EItemType::cloth || GetItemTypeBySlot(FromInventorySlot) != EItemType::weapon)
		{
			UE_LOG(LogTemp, Warning, TEXT("ITEM IS NOT EQUIPPABLE"))
				return;
		}

		if (GetEquipmentTypeBySlot(ToInventorySlot) != localEquipmentSlot)
		{
			UE_LOG(LogTemp, Warning, TEXT("ITEM CAN NOT BE EQUIPPED IN THAT SLOT"))
				return;
		}
		AddItem(ToInventory, ToInventorySlot, localInventoryItem);
		AddItem(FromInventory, FromInventorySlot, localSwapInventoryItem);
	}
	else
	{
		if (Cast<UEquipmentComponent>(ToInventory))
		{
			if (ToInventorySlot < (uint8)EEquipmentSlot::Count)
			{
				if (GetEquipmentTypeBySlot(ToInventorySlot) != localEquipmentSlot)
				{
					UE_LOG(LogTemp, Warning, TEXT("ITEM CAN NOT BE EQUIPPED IN THAT SLOT"))
					return;
				}
			}
		}
		AddItem(ToInventory, ToInventorySlot, localInventoryItem);
		RemoveItem(FromInventory, FromInventorySlot);
	}
	UpdateEquippedStats();
}

void UInventoryManagerComponent::MoveItem(UInventoryComponent* FromInventory, uint8 FromInventorySlot, UInventoryComponent* ToInventory, uint8 ToInventorySlot)
{
	if (FromInventory == ToInventory && FromInventorySlot == ToInventorySlot)
	{
		return;
	}
	FSlotStructure localInventoryItem = FromInventory->GetInventoryItem(FromInventorySlot);
	FSlotStructure localSwapInventoryItem = ToInventory->GetInventoryItem(ToInventorySlot);
	if (!CanContainerStoreItems(ToInventory))
	{
		return;
	}
	if (localInventoryItem.itemStructure.itemType == EItemType::currency)
	{
		AddGold(localInventoryItem.amount);
		RemoveItem(FromInventory, FromInventorySlot);
		return;
	}
	if (ItemIsValid(localSwapInventoryItem))
	{
		if (ItemIsSame(localInventoryItem, localSwapInventoryItem) && IsItemStackable(localSwapInventoryItem) && ItemFreeStackSpace(localSwapInventoryItem) > 0)
		{
			uint8 amountRemaing = 0;
			AddItemToStack(ToInventory, ToInventorySlot, localInventoryItem.amount, amountRemaing);
			if (amountRemaing > 0)
			{
				localInventoryItem.amount = amountRemaing;
				AddItem(FromInventory, FromInventorySlot, localInventoryItem);
				return;
			}
			else
			{
				RemoveItem(FromInventory, FromInventorySlot);
				return;
			}
		}
		else
		{
			if (!CanContainerStoreItems(FromInventory))
			{
				UE_LOG(LogTemp, Warning, TEXT("CONTAINER CANNOT STORE ITEMS"))
				return;
			}
			else
			{
				AddItem(ToInventory, ToInventorySlot, localInventoryItem);
				AddItem(FromInventory, FromInventorySlot, localSwapInventoryItem);
			}
		}
	}
	else
	{
		AddItem(ToInventory, ToInventorySlot, localInventoryItem);
		RemoveItem(FromInventory, FromInventorySlot);
	}
}

void UInventoryManagerComponent::RemoveItem(UInventoryComponent* Inventory, uint8 InventorySlot)
{
	Inventory->ClearInventoryItem(InventorySlot);
	if (playerInventory == Inventory)
	{
		Client_ClearInventorySlotItem(InventorySlot);
	}
	else
	{
		ClearViewersContainerSlot(InventorySlot);
	}
}

void UInventoryManagerComponent::UseEquipmentItem(uint8 InventorySlot, FSlotStructure InventoryItem, UInventoryComponent* ToInventory)
{
	if (InventorySlot < (uint8)EEquipmentSlot::Count)
	{
		uint8 index = 0;
		if (ToInventory == containerInventory)
		{
			FReturnTupleBoolInt localTuple = ToInventory->GetEmptyContainerSpace();
			index = localTuple.Index;
			if (localTuple.Success)
			{
				UnEquipItem(playerInventory, InventorySlot, ToInventory, index);
				return;
			}
		}
		else if (ToInventory == playerInventory)
		{
			if (ToInventory->GetEmptyInventorySpace(index))
			{
				UnEquipItem(playerInventory, InventorySlot, ToInventory, index);
				return;
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("NO FREE SPACE"))
	}
	else
	{
		EEquipmentSlot localEquipmentSlot = GetItemEquipmentSlot(InventoryItem);
		uint8 outInventorySlot = 0;
		if (GetEmptyEquipmentSlotByType(localEquipmentSlot, outInventorySlot))
		{
			Server_EquipFromInventory(InventorySlot, outInventorySlot);
		}
		else
		{
			outInventorySlot = GetEquipmentSlotByType(localEquipmentSlot);
			Server_EquipFromInventory(InventorySlot, outInventorySlot);
		}
	}
}

void UInventoryManagerComponent::UseConsumableItem(uint8 InventorySlot, FSlotStructure InventoryItem)
{
	bool wasFullAmountRemoved = false;
	uint8 amountToRemove = 1;
	uint8 amountRemoved = 0;
	RemoveFromItemAmount(InventoryItem, amountToRemove, wasFullAmountRemoved, amountRemoved);
	if (wasFullAmountRemoved)
	{
		InventoryItem = GetEmptySlot(EEquipmentSlot::Undefined);
		RemoveItem(playerInventory, InventorySlot);
	}
	else
	{
		AddItem(playerInventory, InventorySlot, InventoryItem);
	}
}

void UInventoryManagerComponent::UseFunitureItem(uint8 InventorySlot, FSlotStructure InventoryItem)
{

}

void UInventoryManagerComponent::UseInventoryItem(const uint8& InventorySlot)
{
	FSlotStructure localInventorySlot = playerInventory->GetInventoryItem(InventorySlot);
	if (IsValid(CurrentContainer))
	{
		bool bCanStoreItems = IHUDInterface::Execute_GetCanStoreItems(CurrentContainer);
		if (bCanStoreItems)
		{
			switch (localInventorySlot.itemStructure.itemType)
			{
			case EItemType::cloth:
				if (InventorySlot < (uint8)EEquipmentSlot::Count)
				{
					UseEquipmentItem(InventorySlot, localInventorySlot, containerInventory);
					break;
				}
			case EItemType::weapon:
				if (InventorySlot < (uint8)EEquipmentSlot::Count)
				{
					UseEquipmentItem(InventorySlot, localInventorySlot, containerInventory);
					break;
				}
			case EItemType::consumable:
			case EItemType::furniture:
			case EItemType::currency:
			default: bool bOutSuccess = false;
				TryToAddItemToInventory(containerInventory, localInventorySlot, bOutSuccess);
				if (bOutSuccess)
				{
					RemoveItem(playerInventory, InventorySlot);
				}
				else
				{
					AddItem(playerInventory, InventorySlot, localInventorySlot);
				}
				break;
			}
			return;
		}
	}
	switch (localInventorySlot.itemStructure.itemType)
	{
	case EItemType::consumable:
		UseConsumableItem(InventorySlot, localInventorySlot);
		break;
	case EItemType::weapon:
		UseEquipmentItem(InventorySlot, localInventorySlot, playerInventory);
		break;
	case EItemType::cloth:
		UseEquipmentItem(InventorySlot, localInventorySlot, playerInventory);
		break;
	default: break;
	}
	Client_CheckHotbarSlots(localInventorySlot);
}

void UInventoryManagerComponent::ClearInventoryItem(const uint8& InventorySlot)
{
	playerInventory->Inventory[InventorySlot] = GetEmptySlot(GetEquipmentTypeBySlot(InventorySlot));
}

void UInventoryManagerComponent::SetInventorySlotItem(const FSlotStructure& ContentToAdd, const uint8& InventorySlot)
{
	if (IsValid(mainLayoutUI))
	{
		UInventoryItemSlot* slotLayout{};
		if (InventorySlot >= (uint8)EEquipmentSlot::Count)
		{
			uint8 localIndex = InventorySlot = (uint8)EEquipmentSlot::Count;
			slotLayout = mainLayoutUI->Inventory->inventorySlotsArray[localIndex];
		}
		else
		{
			slotLayout = mainLayoutUI->Profile->EquipmentSlotArray[InventorySlot];
		}
		slotLayout->UpdateSlot(ContentToAdd);
	}
}

void UInventoryManagerComponent::ClearInventorySlotItem(uint8 InventorySlot)
{
	if (IsValid(mainLayoutUI))
	{
		UInventoryItemSlot* SlotLayout{};
		FSlotStructure LocalSlot{};
		if (InventorySlot >= (uint8)EEquipmentSlot::Count)
		{
			uint8 LocalIndex = InventorySlot - (uint8)EEquipmentSlot::Count;

			SlotLayout = mainLayoutUI->Inventory->InventorySlotsArray[LocalIndex];
			LocalSlot = GetEmptySlot(EEquipmentSlot::Undefined);
		}
		else
		{
			SlotLayout = mainLayoutUI->Profile->EquipmentSlotsArray[InventorySlot];

			switch (InventorySlot)
			{
			case 0: LocalSlot = GetEmptySlot(EEquipmentSlot::Weapon);
				break;
			case 1: LocalSlot = GetEmptySlot(EEquipmentSlot::Head);
				break;
			case 2: LocalSlot = GetEmptySlot(EEquipmentSlot::Top);
				break;
			case 3: LocalSlot = GetEmptySlot(EEquipmentSlot::Bottom);
				break;
			case 4: LocalSlot = GetEmptySlot(EEquipmentSlot::Feet);
				break;
			default: LocalSlot = GetEmptySlot(EEquipmentSlot::Undefined);
				break;
			}
		}

		SlotLayout->UpdateSlot(LocalSlot);

		mainLayoutUI->Inventory->UpdateGoldAmount();
	}
}

FSlotStructure UInventoryManagerComponent::GetInventorySlotItem(uint8 InventorySlot)
{
	uint8 index = InventorySlot;
	if (InventorySlot >= (uint8)EEquipmentSlot::Count)
	{
		index = InventorySlot - (uint8)EEquipmentSlot::Count;
	}
	FSlotStructure slot = mainLayoutUI->Inventory->inventorySlotsArray[index]->SlotStructure;
	return slot;
}

void UInventoryManagerComponent::AddGold(uint8 Amount)
{
	Gold += Amount;
	OnRep_UpdateGoldAmount();
}

void UInventoryManagerComponent::ClearContainerSlots()
{
	mainLayoutUI->Container->ContainerSlotsArray.Empty();
	mainLayoutUI->Container->ContainerGridPanel->ClearChildren();
}

void UInventoryManagerComponent::CreateContainerSlots(uint8 NumberOfRows, uint8 SlotsPerRow)
{
	ClearContainerSlots();
	AVICTIMSPlayerController* pc = Cast<AVICTIMSPlayerController>(GetOwner());
	AMainHUD* widgetLayout = Cast<AMainHUD>(pc->HUD_Reference);
	if (widgetLayout)
	{
		UInventoryItemSlot* w_ContainerSlot = nullptr;
		for (int i = 0; i < NumberOfRows; i++)
		{
			for (int j = 0; j < SlotsPerRow; j++)
			{
				w_ContainerSlot = CreateWidget<UInventoryItemSlot>(GetWorld(), widgetLayout->MainMenuWidget->slot);
				mainLayoutUI->Container->ContainerGridPanel->AddChildToUniformGrid(w_ContainerSlot,i,j);
				mainLayoutUI->Container->ContainerSlotsArray.Add(w_ContainerSlot);
			}
		}
		FSlotStructure slotStructure = GetEmptySlot(EEquipmentSlot::Undefined);
		for (int i = 0; i < mainLayoutUI->Container->ContainerSlotsArray.Num(); i++)
		{
			mainLayoutUI->Container->ContainerSlotsArray[i]->UpdateSlot(slotStructure);
			mainLayoutUI->Container->ContainerSlotsArray[i]->InventorySlotIndex = i;
			mainLayoutUI->Container->ContainerSlotsArray[i]->NativeFromContainer = true;
			mainLayoutUI->Container->ContainerSlotsArray[i]->IsStorageSlot = true;

		}
	}
}

void UInventoryManagerComponent::SetViewersContainerSlot(uint8 ContainerSlot, FSlotStructure& InventoryItem)
{
	TArray<APlayerState*> temPlayersViewing = IHUDInterface::Execute_GetPlayersViewing(CurrentContainer);
	for (APlayerState* playerState : temPlayersViewing)
	{
		if (AVICTIMSPlayerController* pc = Cast<AVICTIMSPlayerController>(playerState->GetOwner()))
		{
			pc->inventoryManagerComponent->Client_SetContainerSlotItem(InventoryItem, ContainerSlot);
		}
	}
}

void UInventoryManagerComponent::ClearContainerSlotItem(uint8 ContainerSlot)
{
	UInventoryItemSlot* slotLayout = mainLayoutUI->Container->ContainerSlotsArray[ContainerSlot];
	slotLayout->UpdateSlot(GetEmptySlot(EEquipmentSlot::Undefined));
}

void UInventoryManagerComponent::ClearViewersContainerSlot(uint8 ContainerSlot)
{
	TArray<APlayerState*> localPlayersViewing = IHUDInterface::Execute_GetPlayersViewing(CurrentContainer);
	for (APlayerState* playerState : localPlayersViewing)
	{
		if (AVICTIMSPlayerController* pc = Cast<AVICTIMSPlayerController>(playerState->GetOwner()))
		{
			pc->inventoryManagerComponent->Client_ClearContainerSlotItem(ContainerSlot);
		}
	}
}

void UInventoryManagerComponent::SetContainerSlotItem(const FSlotStructure& Slot, uint8 Index)
{
	UInventoryItemSlot* slotLayout = mainLayoutUI->Container->ContainerSlotsArray[Index];
	slotLayout->UpdateSlot(Slot);
}

void UInventoryManagerComponent::OpenContainer(AActor* Container)
{
	CurrentContainer = Container;
	UInventoryComponent* GetContainerInventoryTemp{};
	IHUDInterface::Execute_GetContainerInventory(CurrentContainer, GetContainerInventoryTemp);
	containerInventory = GetContainerInventoryTemp;
	TArray<FSlotStructure> localInventory{};
	FSlotStructure localEmptySlot = GetEmptySlot(EEquipmentSlot::Undefined);
	for (FSlotStructure slot : containerInventory->Inventory)
	{
		if (slot.amount == 0)
		{
			localInventory.Add(localEmptySlot);
		}
		else
		{
			localInventory.Add(slot);
		}
	}
	FName localName;
	uint8 localNumberOfRows;
	uint8 localSlotsPerRow;
	bool localIsStorageContainer;
	uint8 localInventorySize;
	IHUDInterface::Execute_GetContainerProperties(container, localName, localNumberOfRows, localSlotsPerRow, localIsStorageContainer, localInventorySize);
	FContainerInfo c_info;
	c_info.containerName = localName;
	c_info.numberOfRows = localNumberOfRows;
	c_info.slotsPerRow = localSlotsPerRow;
	c_info.bIsContainer = localIsStorageContainer;
	c_info.storageSize  localInventorySize;

	Client_OpenContainer(c_info, localInventory, playerInventory->Inventory);
}

void UInventoryManagerComponent::UseContainer(AActor* Container)
{
	if (CurrentContainer != Container)
	{
		OpenContainer(Container);
	}
	else
	{
		Server_CloseContainer();
	}
}

void UInventoryManagerComponent::CloseContainer()
{
	if (AVICTIMSPlayerController* pc = Cast<AVICTIMSPlayerController>(GetOwner()))
	{
		if (AContainerActor* CurrentContainerActor = Cast<AContainerActor>(CurrentContainer))
		{
			CurrentContainerActor->PlayersViewing.Remove(pc->PlayerState);
		}
	}
	CurrentContainer = nullptr;
	containerInventory = nullptr;

	Client_CloseContainer();
}

void UInventoryManagerComponent::LoadContainerSlots(FContainerInfo ContainerProperties, const TArray<FSlotStructure>& InContainerInventory, const TArray<FSlotStructure>& InPlayerInventory)
{
	if (ContainerProperties.bIsContainer)
	{

		mainLayoutUI->Container->bIsStorageContainer = true;
		CreateContainerSlots(ContainerProperties.numberOfRows, ContainerProperties.slotsPerRow);
	}

	uint8 Index = 0;
	for (FSlotStructure Slot : InContainerInventory)
	{
		SetContainerSlotItem(Slot, Index);

		Index++;
	}
}

bool UInventoryManagerComponent::CanContainerStoreItems(UInventoryComponent* Inventory)
{
	if (IsValid(CurrentContainer))
	{
		UInventoryComponent* localInventory{};
		IHUDInterface::Execute_GetContainerInventory(CurrentContainer, localInventory);
		if (Inventory == localInventory)
		{
			bool localCanStoreItems = IHUDInterface::Execute_GetCanStoreItems(CurrentContainer);
			if (!localCanStoreItems)
			{
				return false;
			}
		}
	}
	return true;
}

void UInventoryManagerComponent::FindAndAddAmountToStacks(UInventoryComponent* Inventory, FName ItemID, uint8 Amount, uint8& AmountRemaining)
{
	uint8 LocalAmount = Amount;
	FName LocalItemID = ItemID;
	UInventoryComponent* LocalInventory = Inventory;

	uint8 LocalInventorySlot = 0;
	uint8 InitialIndex = 0;
	if (LocalInventory == playerInventory)
	{
		InitialIndex = (uint8)EEquipmentSlot::Count;
		LocalInventorySlot = (uint8)EEquipmentSlot::Count;
	}

	for (uint8 i = InitialIndex; i < LocalInventory->Inventory.Num(); i++)
	{
		if (LocalItemID == LocalInventory->Inventory[LocalInventorySlot].itemStructure.ID)
		{
			AddItemToStack(LocalInventory, LocalInventorySlot, LocalAmount, AmountRemaining);
			LocalAmount = AmountRemaining;

			if (LocalAmount == 0)
			{
				break;
			}
		}
		LocalInventorySlot++;
	}

	AmountRemaining = LocalAmount;
}

void UInventoryManagerComponent::AddItemToStack(UInventoryComponent* Inventory, uint8 InventorySlot, uint8 AmountToAdd, uint8& AmountRemaining)
{
	uint8 LocalRemainingAmount = AmountToAdd;
	FSlotStructure LocalInventoryItem = Inventory->GetInventoryItem(InventorySlot);

	if (GetItemAmount(LocalInventoryItem) < GetItemMaxStackSize(LocalInventoryItem))
	{
		if (AmountToAdd <= ItemFreeStackSpace(LocalInventoryItem))
		{
			LocalRemainingAmount = 0;

			AddAmountToItem(LocalInventoryItem, AmountToAdd);
			AddItem(Inventory, InventorySlot, LocalInventoryItem);
		}
		else
		{
			LocalRemainingAmount = AmountToAdd - ItemFreeStackSpace(LocalInventoryItem);
			LocalInventoryItem.amount = GetItemMaxStackSize(LocalInventoryItem);
			AddItem(Inventory, InventorySlot, LocalInventoryItem);
		}
	}

	AmountRemaining = LocalRemainingAmount;

}

