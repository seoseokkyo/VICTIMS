
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EEquipmentSlot.h"
#include "FSlotStructure.h"
#include "Tuples.h"
#include "InventoryComponent.generated.h"

class UDataTable;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VICTIMS_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UInventoryComponent();			
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="NumberOfRows", Category="Inventory UI"))		// 기본 슬롯 개수 초기화
	uint8 NumberOfRowsInventory = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="SlotsPerRow", Category="Inventory UI"))
	uint8 SlotsPerRowInventory = 0;

	uint8 NumberOfSlotsOnHotbar = 0;																			// 퀵슬롯 개수

	TArray<FSlotStructure> Inventory;																			// 인벤토리 슬롯단위 데이터 모음 

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	UDataTable* ItemDB;																							// 아이템 데이터테이블 
public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(Server, Reliable)
	virtual void Server_InitInventory(uint8 InventorySize);														// 서버_인벤토리 초기화
	UFUNCTION()
	void InitInventory(uint8 InventorySize);																	// 실제 인벤토리 초기화

	UFUNCTION()
	bool LoadInventoryItems(uint8 Size, TArray<FSlotStructure> Array);											// 인벤토리 아이템 로드

	UFUNCTION()
	virtual void ClearInventoryItem(uint8 InventorySlot);														// 인벤토리 아이템 지우기(초기화)
	UFUNCTION()
	virtual void SetInventoryItem(uint8 InventorySlot, FSlotStructure& Item);									// 인벤토리 아이템 세팅

	UFUNCTION()
	FSlotStructure GetInventorySlot(uint8 InventorySlot);														// 인벤토리 슬롯 가져오기 
	UFUNCTION()
	EEquipmentSlot GetEquipmentTypeBySlot(uint8 InventorySlot);													// 장비타입 슬롯 가져오기
	
	UFUNCTION()
	void GetInventoryItems(TArray<FSlotStructure>& InventoryItems);												// 인벤토리 아이템 가져오기

	UFUNCTION()
	bool ItemIsValid(const FSlotStructure& Slot);																// 슬롯 아이템 유효성 검사

	FReturnTupleBoolInt GetEmptyInventorySpace();																// 인벤토리 빈칸 가져오기
	FReturnTupleBoolInt GetEmptyContainerSpace();																// 보관함 빈칸 가져오기
	bool GetEmptyInventorySpace(uint8& OutIndex);																// 빈칸 있는지 확인

	UFUNCTION()
	FSlotStructure GetInventoryItem(uint8 InventorySlot);														// 인벤토리 아이템 가져오기

	UFUNCTION()
	FSlotStructure GetEmptySlot(EEquipmentSlot FromEquipmentType);												// 빈 슬롯 가져오기
	UFUNCTION()
	FSlotStructure GetItemFromItemDB(FName Name);																// 데이터 이름으로 할당
};
