

#include "InventoryComponent.h"
#include "ItemBase.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h>

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;											
}
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// 아이템데이터테이블 정보 받아두기 
	UDataTable* bp_itemDB = LoadObject<UDataTable>(this, TEXT("/Script/Engine.DataTable'/Game/Item/Data/BP_ItemData.BP_ItemData'"));
	if (IsValid(bp_itemDB))
	{
		itemDB = bp_itemDB;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemData not Loaded"))
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
	// 장비슬롯 빈칸 세팅

	Inventory.Reserve(InventorySize);
	FSlotStructure slotStructure = {};
	Inventory.Init(slotStructure, InventorySize);

	for (uint8 index = 0; index < Inventory.Num(); index++)
	{
		if (index == 0)
		{
			slotStructure = GetEmptySlot(EEquipmentSlot::Weapon);
		}
		else if (index == 1)
		{
			slotStructure = GetEmptySlot(EEquipmentSlot::Head);
		}
		else if (index == 2)
		{
			slotStructure = GetEmptySlot(EEquipmentSlot::Top);
		}
		else if (index == 3)
		{
			slotStructure = GetEmptySlot(EEquipmentSlot::Bottom);
		}
		else if (index == 4)
		{
			slotStructure = GetEmptySlot(EEquipmentSlot::Feet);
		}
		else
		{
			slotStructure = GetEmptySlot(EEquipmentSlot::Undefined);
		}
		Inventory[index] = slotStructure;
	}
}

bool UInventoryComponent::LoadInventoryItems(uint8 Size, TArray<FSlotStructure> Array)
{
	Inventory.Reset();
	FSlotStructure tempSlot = {};

	FSlotStructure slotStructure = GetEmptySlot(EEquipmentSlot::Undefined);	// 디폴트 빈칸 슬롯값 가져옴

	for (uint8 i = 0; i <= Size - 1; i++)
	{
		Inventory.Add(tempSlot);		
	}

	uint8 ArrayIndex = 0;
	for (FSlotStructure slot : Array)
	{
		SetInventoryItem(ArrayIndex, slot);
		ArrayIndex++;							
	}

	return true;
}

void UInventoryComponent::ClearInventoryItem(uint8 InventorySlot)
{
	EEquipmentSlot equipmentSlot = EEquipmentSlot::Undefined;					// 장비창 데이터 지우기
	if (InventorySlot < (uint8)EEquipmentSlot::Count)
	{
		equipmentSlot = Inventory[InventorySlot].itemStructure.equipmentSlot;
	}
	Inventory[InventorySlot] = GetEmptySlot(equipmentSlot);
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
	return Inventory[InventorySlot].itemStructure.equipmentSlot;
}

void UInventoryComponent::GetInventoryItems(TArray<FSlotStructure>& InventoryItems)
{
	InventoryItems = Inventory;		
}

bool UInventoryComponent::ItemIsValid(const FSlotStructure& Slot)
{
	if (Slot.amount > 0)
	{
		return true;
	}
	return false;
}

FReturnTupleBoolInt UInventoryComponent::GetEmptyInventorySpace()
{
	int8 localInteger = -1;
	bool localBoolean = false;

	for (uint8 ArrayIndex = (uint8)EEquipmentSlot::Count; ArrayIndex < Inventory.Num(); ArrayIndex++)
	{
		FSlotStructure slot = Inventory[ArrayIndex];
		if (!ItemIsValid(slot))
		{
			localInteger = ArrayIndex;
			localBoolean = true;
			break;
		}
	}

	if (localBoolean)
	{
		return { true, localInteger };
	}
	return { false, 0 };
}

FReturnTupleBoolInt UInventoryComponent::GetEmptyContainerSpace()
{
	int8 localInteger = -1;
	bool localBoolean = false;

	for (uint8 ArrayIndex = 0; ArrayIndex < Inventory.Num(); ArrayIndex++)
	{
		FSlotStructure slot = Inventory[ArrayIndex];
		if (!ItemIsValid(slot))
		{
			localInteger = ArrayIndex;
			localBoolean = true;
		}
	}

	if (localBoolean)
	{
		return { true, localInteger };
	}
	return { false, 0 };
}

bool UInventoryComponent::GetEmptyInventorySpace(uint8& OutIndex)
{
	for (uint8 index = (uint8)EEquipmentSlot::Count; index < Inventory.Num(); index++)
	{
		FSlotStructure slot = Inventory[index];
		if (!ItemIsValid(slot))
		{
			OutIndex = index;
			return true;
		}
	}
	return false;
}

FSlotStructure UInventoryComponent::GetInventoryItem(uint8 InventorySlot)
{
	if (Inventory.Num() > 0)
	{
		if (Inventory[InventorySlot].amount > 0)
		{
			return Inventory[InventorySlot];
		}
	}
	return GetEmptySlot(EEquipmentSlot::Undefined);
}

FSlotStructure UInventoryComponent::GetEmptySlot(EEquipmentSlot FromEquipmentType)
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
	else if(FromEquipmentType == EEquipmentSlot::Bottom)
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

FSlotStructure UInventoryComponent::GetItemFromItemDB(FName Name)
{
	FSlotStructure slot = {};
	const UDataTable* itemTable = itemDB;
	const FItemData* newItemData = itemTable->FindRow<FItemData>(Name, "", true);

	slot.InitSlot(*newItemData, 0);

	return slot;
}
