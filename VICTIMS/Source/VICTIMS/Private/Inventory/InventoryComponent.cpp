

#include "InventoryComponent.h"
#include "AVICTIMSPlayerController.h"
#include "Inventory/InventoryManagerComponent.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	UDataTable* BP_ItemDB = LoadObject<UDataTable>(this, TEXT("/Script/Engine.DataTable'/Game/Item/Data/Item_DB.Item_DB'"));
	if (IsValid(BP_ItemDB))
	{
		ItemDB = BP_ItemDB;
	}
	else
	{
// 		UE_LOG(LogTemp, Warning, TEXT("UDataTable not Loaded"))
	}
}

void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UInventoryComponent::Server_InitInventory_Implementation(uint8 InventorySize)
{
	InitInventory(InventorySize);
}

void UInventoryComponent::InitInventory(uint8 InventorySize)
{
	Inventory.Reserve(InventorySize);

	FSlotStructure SlotStructure = {};
	Inventory.Init(SlotStructure, InventorySize);

	// Add Customized Icons to Slots
	for (uint8 Index = 0; Index < Inventory.Num(); Index++)
	{
		if (Index == 0)
		{
			SlotStructure = GetEmptySlot(EEquipmentSlot::Weapon);
		}
		else if (Index == 1)
		{
			SlotStructure = GetEmptySlot(EEquipmentSlot::Head);
		}
		else if (Index == 2)
		{
			SlotStructure = GetEmptySlot(EEquipmentSlot::Chest);
		}
		else if (Index == 3)
		{
			SlotStructure = GetEmptySlot(EEquipmentSlot::Legs);
		}
		else if (Index == 4)
		{
			SlotStructure = GetEmptySlot(EEquipmentSlot::Feet);
		}
		else
		{
			SlotStructure = GetEmptySlot(EEquipmentSlot::Undefined);
		}
		Inventory[Index] = SlotStructure;
	}
}

bool UInventoryComponent::LoadInventoryItems(uint8 Size, TArray<FSlotStructure> Array)
{
	Inventory.Reset();
	FSlotStructure TempSlot = {};

	FSlotStructure SlotStructure = GetEmptySlot(EEquipmentSlot::Undefined);

	for (uint8 i = 0; i <= Size - 1; i++)
	{
		Inventory.Add(TempSlot);
	}

	uint8 ArrayIndex = 0;
	for (FSlotStructure Slot : Array)
	{
		SetInventoryItem(ArrayIndex, Slot);
		ArrayIndex++;
	}
	return true;
}

void UInventoryComponent::ClearInventoryItem(uint8 InventorySlot)
{
	EEquipmentSlot EquipmentSlot = EEquipmentSlot::Undefined;
	if (InventorySlot < (uint8)EEquipmentSlot::Count)
	{
		EquipmentSlot = Inventory[InventorySlot].ItemStructure.EquipmentSlot;
	}
	Inventory[InventorySlot] = GetEmptySlot(EquipmentSlot);
}

void UInventoryComponent::SetInventoryItem(uint8 InventorySlot, FSlotStructure& Item)
{
	Inventory[InventorySlot] = Item;
}

FSlotStructure UInventoryComponent::GetInventorySlot(uint8 InventorySlot)
{
	return Inventory[InventorySlot];
}

EEquipmentSlot UInventoryComponent::GetEquipmentTypeBySlot(uint8 InventorySlot)
{
	return Inventory[InventorySlot].ItemStructure.EquipmentSlot;
}

void UInventoryComponent::GetInventoryItems(TArray<FSlotStructure>& InventoryItems)
{
	InventoryItems = Inventory;
}

bool UInventoryComponent::ItemIsValid(const FSlotStructure& Slot)
{
	if (Slot.Amount > 0)
	{
		return true;
	}
	return false;
}
FReturnTupleBoolInt UInventoryComponent::GetEmptyInventorySpace()
{
	int8 LocalInteger = -1;
	bool LocalBoolean = false;

	if (uint8 ArrayIndex = (uint8)EEquipmentSlot::Count > Inventory.Num())
	{
		AVICTIMSPlayerController* PC = Cast<AVICTIMSPlayerController>(GetOwner());
		if (PC)
		{
			Server_InitInventory_Implementation(PC->InventoryManagerComponent->TotalNumberOfSlots);
		//	InitInventory(PC->InventoryManagerComponent->TotalNumberOfSlots);
		}
	}
	for (uint8 ArrayIndex = (uint8)EEquipmentSlot::Count; ArrayIndex < Inventory.Num(); ArrayIndex++)	
	{
		FSlotStructure Slot = Inventory[ArrayIndex];
		if (!ItemIsValid(Slot))
		{
			LocalInteger = ArrayIndex;
			LocalBoolean = true;
			break;
		}
	}

	if (LocalBoolean)
	{
		return { true, LocalInteger };
	}
	return { false, 0 };
}

bool UInventoryComponent::GetEmptyInventorySpace(uint8& OutIndex)
{
	for (uint8 Index = (uint8)EEquipmentSlot::Count; Index < Inventory.Num(); Index++)
	{
		FSlotStructure Slot = Inventory[Index];
		if (!ItemIsValid(Slot))
		{
			OutIndex = Index;
			return true;
		}
	}
	return false;
}

FSlotStructure UInventoryComponent::GetInventoryItem(uint8 InventorySlot)
{
	if (Inventory.Num() > 0)
	{
		if (Inventory[InventorySlot].Amount > 0)
		{
			return Inventory[InventorySlot];
		}
	}
	return GetEmptySlot(EEquipmentSlot::Undefined);
}

FSlotStructure UInventoryComponent::GetEmptySlot(EEquipmentSlot FromEquipmentType)
{
	FName Name;
	if (FromEquipmentType == EEquipmentSlot::Weapon)
	{
		Name = "No_Weapon";
	}
	else if (FromEquipmentType == EEquipmentSlot::Chest)
	{
		Name = "No_Chest";
	}
	else if (FromEquipmentType == EEquipmentSlot::Feet)
	{
		Name = "No_Feet";
	}
	else if (FromEquipmentType == EEquipmentSlot::Legs)
	{
		Name = "No_Legs";
	}
	else if (FromEquipmentType == EEquipmentSlot::Head)
	{
		Name = "No_Helmet";
	}
	else
	{
		Name = "Empty";
	}
	return GetItemFromItemDB(Name);
}

FSlotStructure UInventoryComponent::GetItemFromItemDB(FName Name)
{
	FSlotStructure Slot = {};

	const UDataTable* ItemTable = ItemDB;
	const FItemStructure* NewItemData = ItemTable->FindRow<FItemStructure>(Name, "", true);

	Slot.InitSlot(*NewItemData, 0);

	return Slot;
}

FReturnTupleBoolInt UInventoryComponent::GetEmptyContainerSpace()
{
	int8 LocalInteger = -1;
	bool LocalBoolean = false;

	for (uint8 ArrayIndex = 0; ArrayIndex < Inventory.Num(); ArrayIndex++)
	{
		FSlotStructure Slot = Inventory[ArrayIndex];
		if (!ItemIsValid(Slot))
		{
			LocalInteger = ArrayIndex;
			LocalBoolean = true;
			break;
		}
	}
	if (LocalBoolean)
	{
		return { true, LocalInteger };
	}
	return { false, 0 };
}

FReturnTupleBoolInt UInventoryComponent::GetEmptyShopSpace()
{
	int8 LocalInteger = -1;
	bool LocalBoolean = false;
	for(uint8 ArrayIndex = 0; ArrayIndex < Inventory.Num(); ArrayIndex++)
	{
		FSlotStructure Slot = Inventory[ArrayIndex];
		if (!ItemIsValid(Slot))
		{
			LocalInteger = ArrayIndex;
			LocalBoolean = true;
			break;
		}
	}
	if (LocalBoolean)
	{
		return {true, LocalInteger};
	}
	return {false, 0};
}

