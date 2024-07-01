

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
		ItemDB = bp_ItemDB;
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
	TArray<UInventoryItemSlot>
}
void UInventoryManagerComponent::InitializeInventoryManagerUI(UMainMenu* MainLayout)
{
}
EEquipmentSlot UInventoryManagerComponent::GetEquipmentTypeBySlot(const uint8& EquipmentSlot)
{
	return EEquipmentSlot();
}
EItemType UInventoryManagerComponent::GetItemTypeBySlot(uint8 ItemSlot)
{
	return EItemType();
}
void UInventoryManagerComponent::AddItem(UInventoryComponent* Inventory, uint8 InventorySlot, FSlotStructure& InventoryItem)
{
}
FSlotStructure UInventoryManagerComponent::GetEmptySlot(EEquipmentSlot FromEquipmentType)
{
	return FSlotStructure();
}
FSlotStructure UInventoryManagerComponent::GetItemFromItemDB(FName Name)
{
	return FSlotStructure();
}
void UInventoryManagerComponent::UpdateEquippedStats()
{
}
UDataTable* UInventoryManagerComponent::GetItemDB()
{
	return nullptr;
}

void UInventoryManagerComponent::MoveHotbarSlotItem(const uint8& FromSlot, const uint8& ToSlot, const bool IsDraggedFromInventory, const bool IsDraggedFromHotbar)
{
}

void UInventoryManagerComponent::UseHotbarSlot(const uint8& HotbarSlot)
{
}

void UInventoryManagerComponent::ClearHotbarSlotItem(const uint8& HotbarSlot)
{
}

void UInventoryManagerComponent::SetHotbarSlotItem(const uint8& ToSlot, FSlotStructure SlotStructure)
{
}

FSlotStructure UInventoryManagerComponent::GetHotbarSlotItem(const uint8& HotbarSlot)
{
	return FSlotStructure();
}

uint8 UInventoryManagerComponent::GetEquipmentSlotByType(EEquipmentSlot EquipmentSlot)
{
	return uint8();
}

EEquipmentSlot UInventoryManagerComponent::GetItemEquipmentSlot(FSlotStructure Item)
{
	return EEquipmentSlot();
}

bool UInventoryManagerComponent::ItemIsValid(FSlotStructure Item)
{
	return false;
}

bool UInventoryManagerComponent::GetEmptyEquipmentSlotByType(EEquipmentSlot EquipmentSlot, uint8& OutIndex)
{
	return false;
}

bool UInventoryManagerComponent::ItemIsSame(const FSlotStructure Item1, const FSlotStructure Item2)
{
	return false;
}

bool UInventoryManagerComponent::IsItemStackable(const FSlotStructure Item)
{
	return false;
}

uint8 UInventoryManagerComponent::ItemFreeStackSpace(const FSlotStructure Item)
{
	return uint8();
}

uint8 UInventoryManagerComponent::GetItemAmount(const FSlotStructure Item)
{
	return uint8();
}

uint8 UInventoryManagerComponent::GetItemMaxStackSize(const FSlotStructure Item)
{
	return uint8();
}

void UInventoryManagerComponent::AddAmountToItem(FSlotStructure& Item, uint8 AmountToAdd)
{
}

void UInventoryManagerComponent::RemoveFromItemAmount(FSlotStructure& InventoryItem, const uint8& AmountToRemove, bool& WasFullAmountRemoved, uint8& AmountRemoved)
{
}

void UInventoryManagerComponent::EquipItem(UInventoryComponent* FromInventory, uint8 FromInventorySlot, UInventoryComponent* ToInventory, uint8 ToInventorySlot)
{
}

void UInventoryManagerComponent::UnEquipItem(UInventoryComponent* FromInventory, uint8 FromInventorySlot, UInventoryComponent* ToInventory, uint8 ToInventorySlot)
{
}

void UInventoryManagerComponent::RandomizeDropLocation(FSlotStructure LocalSlot, UClass*& LocalClass, FTransform& OutTransform)
{
}

void UInventoryManagerComponent::DropItem(UInventoryComponent* Inventory, uint8 InventorySlot)
{
}

void UInventoryManagerComponent::MoveItem(UInventoryComponent* FromInventory, uint8 FromInventorySlot, UInventoryComponent* ToInventory, uint8 ToInventorySlot)
{
}

void UInventoryManagerComponent::RemoveItem(UInventoryComponent* Inventory, uint8 InventorySlot)
{
}

void UInventoryManagerComponent::UseEquipmentItem(uint8 InventorySlot, FSlotStructure InventoryItem, UInventoryComponent* ToInventory)
{
}

void UInventoryManagerComponent::UseConsumableItem(uint8 InventorySlot, FSlotStructure InventoryItem)
{
}

void UInventoryManagerComponent::UseFunitureItem(uint8 InventorySlot, FSlotStructure InventoryItem)
{
}

void UInventoryManagerComponent::UseInventoryItem(const uint8& InventorySlot)
{
}

void UInventoryManagerComponent::ClearInventoryItem(const uint8& InventorySlot)
{
}

void UInventoryManagerComponent::SetInventorySlotItem(const FSlotStructure& ContentToAdd, const uint8& InventorySlot)
{
}

void UInventoryManagerComponent::ClearInventorySlotItem(uint8 InventorySlot)
{
}

FSlotStructure UInventoryManagerComponent::GetInventorySlotItem(uint8 InventorySlot)
{
	return FSlotStructure();
}

void UInventoryManagerComponent::AddGold(uint8 Amount)
{
}

void UInventoryManagerComponent::ClearContainerSlots()
{
}

void UInventoryManagerComponent::CreateContainerSlots(uint8 NumberOfRows, uint8 SlotsPerRow)
{
}

void UInventoryManagerComponent::SetViewersContainerSlot(uint8 ContainerSlot, FSlotStructure& InventoryItem)
{
}

void UInventoryManagerComponent::ClearContainerSlotItem(uint8 ContainerSlot)
{
}

void UInventoryManagerComponent::ClearViewersContainerSlot(uint8 ContainerSlot)
{
}

void UInventoryManagerComponent::SetContainerSlotItem(const FSlotStructure& Slot, uint8 Index)
{
}

void UInventoryManagerComponent::OpenContainer(AActor* Container)
{
}

void UInventoryManagerComponent::UseContainer(AActor* Container)
{
}

void UInventoryManagerComponent::CloseContainer()
{
}

void UInventoryManagerComponent::LoadContainerSlots(FContainerInfo ContainerProperties, const TArray<FSlotStructure>& InContainerInventory, const TArray<FSlotStructure>& InPlayerInventory)
{
}

bool UInventoryManagerComponent::CanContainerStoreItems(UInventoryComponent* Inventory)
{
	return false;
}

void UInventoryManagerComponent::FindAndAddAmountToStacks(UInventoryComponent* Inventory, FName ItemID, uint8 Amount, uint8& AmountRemaining)
{
}

void UInventoryManagerComponent::AddItemToStack(UInventoryComponent* Inventory, uint8 InventorySlot, uint8 AmountToAdd, uint8& AmountRemaining)
{
}

