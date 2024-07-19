
#include "EquipmentComponent.h"
#include "VICTIMSCharacter.h"
#include "FSlotStructure.h"


UEquipmentComponent::UEquipmentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	EquipmentCharacterReference = nullptr;		// ��� ������ ĳ���� �����Ͱ� ����α�
	
	NumberOfRowsInventory = 4;					// �κ��丮 ���� ���� ����
	SlotsPerRowInventory = 4;

	NumberOfSlotsOnHotbar = 5;					// ������ ���� ���� 

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
	Super::ClearInventoryItem(InventorySlot);							// �κ��丮 ���� �����(�ʱ�ȭ)

	UpdateEquippedMeshes(InventorySlot);
}

void UEquipmentComponent::SetInventoryItem(uint8 InventorySlot, FSlotStructure& Item)
{
	Super::SetInventoryItem(InventorySlot, Item);						// ���ĭ ���� ������Ʈ 

	UpdateEquippedMeshes(InventorySlot);
}

void UEquipmentComponent::UpdateEquippedMeshes(uint8 InventorySlot)
{																		// �������� �Ž� ������Ʈ 
	if (IsValid(EquipmentCharacterReference))
	{
		if (InventorySlot < (uint8)EEquipmentSlot::Count)
		{
			FSlotStructure Slot = GetInventoryItem(InventorySlot);
			USkeletalMesh* NewMesh = Slot.ItemStructure.SkeletalMesh;
			switch (InventorySlot)
			{
			case (uint8)EEquipmentSlot::Weapon:
	
// 				EquipmentCharacterReference->MainWeaponMesh = NewMesh;
// 				EquipmentCharacterReference->OnRep_MainWeaponMesh();

				break;
			case (uint8)EEquipmentSlot::Chest:
				ChestMesh = EquipmentCharacterReference->FindComponentByTag<USkeletalMeshComponent>(FName("Chest"));
				ChestMesh->SetSkeletalMesh(NewMesh);

// 				EquipmentCharacterReference->ChestMesh = NewMesh;
// 				EquipmentCharacterReference->OnRep_MainChestMesh();
				break;
			case (uint8)EEquipmentSlot::Feet:
				FeetMesh = EquipmentCharacterReference->FindComponentByTag<USkeletalMeshComponent>(FName("Feet"));
				FeetMesh->SetSkeletalMesh(NewMesh);

// 				EquipmentCharacterReference->FeetMesh = NewMesh;
// 				EquipmentCharacterReference->OnRep_MainFeetMesh();
				break;
			case (uint8)EEquipmentSlot::Hands:
				EquipmentCharacterReference->HandsMesh = NewMesh;
				EquipmentCharacterReference->OnRep_MainHandsMesh();
				break;
			case (uint8)EEquipmentSlot::Legs:
				BottomMesh = EquipmentCharacterReference->FindComponentByTag<USkeletalMeshComponent>(FName("Bottom"));
				BottomMesh->SetSkeletalMesh(NewMesh);
// 
// 				EquipmentCharacterReference->LegsMesh = NewMesh;
// 				EquipmentCharacterReference->OnRep_MainLegsMesh();
				break;
			case (uint8)EEquipmentSlot::Head:
				HeadMesh = EquipmentCharacterReference->FindComponentByTag<USkeletalMeshComponent>(FName("Head"));
				HeadMesh->SetSkeletalMesh(NewMesh);

// 				EquipmentCharacterReference->HeadMesh = NewMesh;
//  				EquipmentCharacterReference->OnRep_MainHeadMesh();
				break;
			default:
				break;
			}
		}
	}
}
