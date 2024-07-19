
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
