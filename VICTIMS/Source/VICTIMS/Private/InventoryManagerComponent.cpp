

#include "InventoryManagerComponent.h"

UInventoryManagerComponent::UInventoryManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}
void UInventoryManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
}
void UInventoryManagerComponent::OnRep_UpdateGoldAmount()
{
}
void UInventoryManagerComponent::InitializeItemDB()
{
}
void UInventoryManagerComponent::BeginPlay()
{
	Super::BeginPlay();
}
void UInventoryManagerComponent::TryToAddItemToInventory(UInventoryComponent* Inventory, FSlotStructure& InventoryItem, bool& bOutSuccess)
{
}
void UInventoryManagerComponent::Server_UseInventoryItem(const uint8& InventorySlot)
{
}
void UInventoryManagerComponent::Server_UseContainerItem(const uint8& InventorySlot)
{
}
void UInventoryManagerComponent::Server_MoveInventoryItem(uint8 FromInventorySlot, uint8 ToInventorySlot)
{
}
void UInventoryManagerComponent::Server_EquipFromInventory(uint8 FromInventorySlot, uint8 ToInventorySlot)
{
}
void UInventoryManagerComponent::Server_UnEquipToInventory(uint8 FromInventorySlot, uint8 ToInventorySlot)
{
}
void UInventoryManagerComponent::Server_DropItemFromInventory(const uint8& InventorySlot)
{
}
void UInventoryManagerComponent::Server_UseContainer(AActor* Container)
{
}
void UInventoryManagerComponent::Server_CloseContainer()
{
}
void UInventoryManagerComponent::Client_OpenContainer(FContainerInfo ContainerProperties, const TArray<FSlotStructure>& InContainerInventory, const TArray<FSlotStructure>& InPlayerInventory)
{
}
void UInventoryManagerComponent::Client_CloseContainer()
{
}
void UInventoryManagerComponent::Server_TakeContainerItem(const uint8& FromContainerSlot, const uint8& ToInventorySlot)
{
}
void UInventoryManagerComponent::Server_DepositContainerItem(const uint8& FromInventorySlot, const uint8& ToInventorySlot)
{
}
void UInventoryManagerComponent::Server_MoveContainerItem(uint8 FromInventorySlot, uint8 ToInventorySlot)
{
}
void UInventoryManagerComponent::Client_SetInventorySlotItem(const FSlotStructure& ContentToAdd, const uint8& InventorySlot)
{
}
void UInventoryManagerComponent::Client_SetContainerSlotItem(const FSlotStructure& ContentToAdd, const uint8& InventorySlot)
{
}
void UInventoryManagerComponent::Client_ClearInventorySlotItem(uint8 InventorySlot)
{
}
void UInventoryManagerComponent::Client_ClearContainerSlotItem(uint8 ContainerSlot)
{
}
void UInventoryManagerComponent::Client_ClearHotbarSlot(const uint8& HotbarSlot)
{
}
void UInventoryManagerComponent::Client_MoveHotbarSlotItem(const uint8& FromSlot, const uint8& ToSlot, const bool IsDraggedFromInventory, const bool IsDraggedFromHotbar)
{
}
void UInventoryManagerComponent::Client_UseHotbarSlot(const uint8& HotbarSlot)
{
}
void UInventoryManagerComponent::Client_LoadInventoryUI()
{
}
void UInventoryManagerComponent::Client_LoadProfileUI()
{
}
void UInventoryManagerComponent::Client_LoadHotbarUI()
{
}
void UInventoryManagerComponent::LoadHotbarUI()
{
}
void UInventoryManagerComponent::Client_CheckHotbarSlots(const FSlotStructure& Slot)
{
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
void UInventoryManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UInventoryManagerComponent::InitializeInventoryManager(UInventoryComponent* EquipmentComponent)
{
}

void UInventoryManagerComponent::Server_InitInventory()
{
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

