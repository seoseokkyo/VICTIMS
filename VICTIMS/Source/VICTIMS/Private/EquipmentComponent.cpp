

#include "EquipmentComponent.h"
#include "VICTIMSCharacter.h"
#include "FSlotStructure.h"

UEquipmentComponent::UEquipmentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	EquipmentCharacterReference = nullptr;

	NumberOfRowsInventory = 7;
	SlotsPerRowInventory = 4;

	NumberOfSlotsOnHotbar = 5;
}

void UEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UEquipmentComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UEquipmentComponent::ClearInventoryItem(uint8 InventorySlot)
{
	Super::ClearInventoryItem(InventorySlot);
	UpdateEquippedMeshes(InventorySlot);
}

void UEquipmentComponent::SetInventoryItem(uint8 InventorySlot, FSlotStructure& Item)
{
	Super::SetInventoryItem(InventorySlot, Item);
	UpdateEquippedMeshes(InventorySlot);
}

void UEquipmentComponent::UpdateEquippedMeshes(uint8 InventorySlot)
{
	if (IsValid(EquipmentCharacterReference))
	{
		if (InventorySlot < (uint8)EEquipmentSlot::Count)
		{
			FSlotStructure slot = GetInventoryItem(InventorySlot);
			USkeletalMesh* newMesh = slot.itemStructure.assetData.skeletalMesh;

			switch (InventorySlot)
			{
			case (uint8)EEquipmentSlot::Weapon:
				EquipmentCharacterReference->MainWeaponMesh = newMesh;
				EquipmentCharacterReference->OnRep_MainWeaponMesh();
				break;
			case (uint8)EEquipmentSlot::Head:
				EquipmentCharacterReference->HeadMesh = newMesh;
				EquipmentCharacterReference->OnRep_MainHeadMesh();
				break;
			case (uint8)EEquipmentSlot::Top:
				EquipmentCharacterReference->TopMesh = newMesh;
				EquipmentCharacterReference->OnRep_MainTopMesh();
				break;
			case (uint8)EEquipmentSlot::Bottom:
				EquipmentCharacterReference->BottomMesh = newMesh;
				EquipmentCharacterReference->OnRep_MainBottomMesh();
				break;
			case (uint8)EEquipmentSlot::Feet:
				EquipmentCharacterReference->FeetMesh = newMesh;
				EquipmentCharacterReference->OnRep_MainFeetMesh();
				break;
			default:
				break;
			}
		}
	}
}
