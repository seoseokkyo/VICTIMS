
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
		ServerRPC_UpdateEquippedMeshes(InventorySlot);
	}
}

void UEquipmentComponent::ServerRPC_UpdateEquippedMeshes_Implementation(uint8 InventorySlot)
{
	FSlotStructure Slot = GetInventoryItem(InventorySlot);
	USkeletalMesh* NewMesh = Slot.ItemStructure.SkeletalMesh;

	NetMulticastRPC_UpdateEquippedMeshes(InventorySlot, Slot, NewMesh);
}

void UEquipmentComponent::NetMulticastRPC_UpdateEquippedMeshes_Implementation(uint8 InventorySlot, FSlotStructure Slot, USkeletalMesh* NewMesh)
{
	if (IsValid(EquipmentCharacterReference))
	{
		switch (Slot.ItemStructure.EquipmentSlot)
		{
		case EEquipmentSlot::Weapon:
			break;
		case EEquipmentSlot::Chest:
			ChestMesh = EquipmentCharacterReference->FindComponentByTag<USkeletalMeshComponent>(FName("Chest"));
			ChestMesh->SetSkeletalMesh(NewMesh);
			break;
		case EEquipmentSlot::Feet:
			FeetMesh = EquipmentCharacterReference->FindComponentByTag<USkeletalMeshComponent>(FName("Feet"));
			FeetMesh->SetSkeletalMesh(NewMesh);
			break;
		case EEquipmentSlot::Hands:
			EquipmentCharacterReference->HandsMesh = NewMesh;
			EquipmentCharacterReference->OnRep_MainHandsMesh();
			break;
		case EEquipmentSlot::Legs:
			BottomMesh = EquipmentCharacterReference->FindComponentByTag<USkeletalMeshComponent>(FName("Bottom"));
			BottomMesh->SetSkeletalMesh(NewMesh);
			break;
		case EEquipmentSlot::Head:
			HeadMesh = EquipmentCharacterReference->FindComponentByTag<USkeletalMeshComponent>(FName("Head"));
			HeadMesh->SetSkeletalMesh(NewMesh);
			break;
		default:
			break;
		}		
	}
}
