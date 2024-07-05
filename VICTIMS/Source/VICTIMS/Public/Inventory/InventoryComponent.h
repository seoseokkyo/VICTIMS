
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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="NumberOfRows", Category="Inventory UI"))		// �⺻ ���� ���� �ʱ�ȭ
	uint8 NumberOfRowsInventory = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="SlotsPerRow", Category="Inventory UI"))
	uint8 SlotsPerRowInventory = 0;

	uint8 NumberOfSlotsOnHotbar = 0;																			// ������ ����

	TArray<FSlotStructure> Inventory;																			// �κ��丮 ���Դ��� ������ ���� 

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	UDataTable* ItemDB;																							// ������ ���������̺� 
public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(Server, Reliable)
	virtual void Server_InitInventory(uint8 InventorySize);														// ����_�κ��丮 �ʱ�ȭ
	UFUNCTION()
	void InitInventory(uint8 InventorySize);																	// ���� �κ��丮 �ʱ�ȭ

	UFUNCTION()
	bool LoadInventoryItems(uint8 Size, TArray<FSlotStructure> Array);											// �κ��丮 ������ �ε�

	UFUNCTION()
	virtual void ClearInventoryItem(uint8 InventorySlot);														// �κ��丮 ������ �����(�ʱ�ȭ)
	UFUNCTION()
	virtual void SetInventoryItem(uint8 InventorySlot, FSlotStructure& Item);									// �κ��丮 ������ ����

	UFUNCTION()
	FSlotStructure GetInventorySlot(uint8 InventorySlot);														// �κ��丮 ���� �������� 
	UFUNCTION()
	EEquipmentSlot GetEquipmentTypeBySlot(uint8 InventorySlot);													// ���Ÿ�� ���� ��������
	
	UFUNCTION()
	void GetInventoryItems(TArray<FSlotStructure>& InventoryItems);												// �κ��丮 ������ ��������

	UFUNCTION()
	bool ItemIsValid(const FSlotStructure& Slot);																// ���� ������ ��ȿ�� �˻�

	FReturnTupleBoolInt GetEmptyInventorySpace();																// �κ��丮 ��ĭ ��������
	FReturnTupleBoolInt GetEmptyContainerSpace();																// ������ ��ĭ ��������
	bool GetEmptyInventorySpace(uint8& OutIndex);																// ��ĭ �ִ��� Ȯ��

	UFUNCTION()
	FSlotStructure GetInventoryItem(uint8 InventorySlot);														// �κ��丮 ������ ��������

	UFUNCTION()
	FSlotStructure GetEmptySlot(EEquipmentSlot FromEquipmentType);												// �� ���� ��������
	UFUNCTION()
	FSlotStructure GetItemFromItemDB(FName Name);																// ������ �̸����� �Ҵ�
};
