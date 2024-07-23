
#include "EquipmentComponent.h"
#include "VICTIMSCharacter.h"
#include "FSlotStructure.h"


UEquipmentComponent::UEquipmentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	EquipmentCharacterReference = nullptr;		// 장비 착용할 캐릭터 데이터값 비워두기
	
	NumberOfRowsInventory = 4;					// 인벤토리 슬롯 개수 설정
	SlotsPerRowInventory = 4;

	NumberOfSlotsOnHotbar = 5;					// 퀵슬롯 개수 설정 

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
	Super::ClearInventoryItem(InventorySlot);							// 인벤토리 내용 지우기(초기화)

	UpdateEquippedMeshes(InventorySlot);
}

void UEquipmentComponent::SetInventoryItem(uint8 InventorySlot, FSlotStructure& Item)
{
	Super::SetInventoryItem(InventorySlot, Item);						// 장비칸 내용 업데이트 

	UpdateEquippedMeshes(InventorySlot);
}

void UEquipmentComponent::UpdateEquippedMeshes(uint8 InventorySlot)
{																		// 장비아이템 매시 업데이트 
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
		switch ((EEquipmentSlot)InventorySlot)
		{
		case EEquipmentSlot::Weapon:
			break;
		case EEquipmentSlot::Chest:
			ChestMesh = EquipmentCharacterReference->Chest;
			if (NewMesh == nullptr)
			{
				ChestMesh->SetSkeletalMesh(EquipmentCharacterReference->DefaultChestMesh, false);
			}
			else
			{
				ChestMesh->SetSkeletalMesh(NewMesh, false);
			}
			break;
		case EEquipmentSlot::Feet:
			FeetMesh = EquipmentCharacterReference->Feet;
			if (NewMesh == nullptr)
			{
				FeetMesh->SetSkeletalMesh(EquipmentCharacterReference->DefaultFeetMesh, false);
			}
			else
			{
				FeetMesh->SetSkeletalMesh(NewMesh, false);
			}
			break;
		case EEquipmentSlot::Hands:
// 			EquipmentCharacterReference->HandsMesh = NewMesh;
// 			EquipmentCharacterReference->OnRep_MainHandsMesh();
			break;
		case EEquipmentSlot::Legs:
			BottomMesh = EquipmentCharacterReference->Legs;
			if (NewMesh == nullptr)
			{
				BottomMesh->SetSkeletalMesh(EquipmentCharacterReference->DefaultLegsMesh, false);
			}
			else
			{
				BottomMesh->SetSkeletalMesh(NewMesh, false);
			}
			break;
		case EEquipmentSlot::Head:
			HeadMesh = EquipmentCharacterReference->Head;
			if (NewMesh == nullptr)
			{
				HeadMesh->SetSkeletalMesh(EquipmentCharacterReference->DefaultHeadMesh, false);
			}
			else
			{
				HeadMesh->SetSkeletalMesh(NewMesh, false);
			}
			break;
		default:
			break;
		}		
	}
}
