
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HUDInterface.h"
#include "EEquipmentSlot.h"
#include "FSlotStructure.h"
#include "InventoryComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnInventoryUpdated);					// 인벤토리 replicate

class UItemBase;
class UDataTable;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VICTIMS_API UInventoryComponent : public UActorComponent, public IHUDInterface
{
	GENERATED_BODY()

public:	

	// 슬롯 가로세로 개수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "NumberOfRows", Category = "Inventory UI"))
	uint8 NumberOfRowsInventory = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "SlotsPerRow", Category = "Inventory UI"))
	uint8 SlotsPerRowInventory = 0;

    // 퀵슬롯인벤 슬롯 개수 
	uint8 NumberOfSlotsOnHotbar = 0;

	TArray<FSlotStructure> Inventory; // 인벤토리 슬롯 정보 

	UPROPERTY()
	UDataTable* itemDB;

	FOnInventoryUpdated onInventoryUpdated;

//======================================================================================
// FUNCTIONS

	UFUNCTION(Server, Reliable)
	virtual void Server_InitInventory(uint8 InventorySize);
	UFUNCTION()
	void InitInventory(uint8 InventorySize);

	UFUNCTION()
	bool LoadInventoryItems(uint8 Size, TArray<FSlotStructure> Array);

	UFUNCTION()
	virtual void ClearInventoryItem(uint8 InventorySlot);
	UFUNCTION()
	virtual void SetInventoryItem(uint8 InventorySlot, FSlotStructure& Item);

	UFUNCTION()
	FSlotStructure GetInventorySlot(uint8 InventorySlot);
	UFUNCTION()
	EEquipmentSlot GetEquipmentTypeBySlot(uint8 InventorySlot);

	UFUNCTION()
	void GetInventoryItems(TArray<FSlotStructure>& InventoryItems);

	UFUNCTION()
	bool ItemIsValid(const FSlotStructure& Slot);

	FReturnTupleBoolInt GetEmptyInventorySpace();
	FReturnTupleBoolInt GetEmptyContainerSpace();
	bool GetEmptyInventorySpace(uint8& OutIndex);

	UFUNCTION()
	FSlotStructure GetInventoryItem(uint8 InventorySlot);

	UFUNCTION()
	FSlotStructure GetEmptySlot(EEquipmentSlot FromEquipmentType);
	UFUNCTION()
	FSlotStructure GetItemFromItemDB(FName Name);


	UInventoryComponent();


protected:

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void BeginPlay() override;

		
};
