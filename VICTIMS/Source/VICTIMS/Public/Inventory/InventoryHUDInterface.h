

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InventoryHUDInterface.generated.h"

UINTERFACE(MinimalAPI)
class UInventoryHUDInterface : public UInterface
{
	GENERATED_BODY()
};


class VICTIMS_API IInventoryHUDInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")							// �κ��丮 ������ �̵�
		void UI_MoveInventoryItem(const uint8& FromInventorySlot, const uint8& ToInventorySlot);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")							// �κ��丮 ������ ���
		void UI_DropInventoryItem(const uint8& InventorySlot);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")							// �κ��丮 ������ ����
		void UI_EquipInventoryItem(const uint8& FromInventorySlot, const uint8& ToInventorySlot);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")							// ������ Ż�� -> �κ��丮 �̵� 
		void UI_UnEquipInventoryItem(const uint8& FromInventorySlot, const uint8& ToInventorySlot);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")							// �κ��丮 ������ ���
		void UI_UseInventoryItem(const uint8& InventorySlot);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")							// ������ ������ ���
		void UI_UseContainerItem(const uint8& InventorySlot);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")							// ������ ������ ���
		void UI_UseHotbarItem(const uint8& InventorySlot);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")							// ������ ������ ������
		void UI_TakeContainerItem(const uint8& FromContainerSlot, const uint8& ToInventorySlot);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")							// �����Կ� ������ ����
		void UI_DepositContainerItem(const uint8& FromInventorySlot, const uint8& ToInventorySlot);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")							// �����Կ��� ������ ����
		void UI_EquipFromContainer(const uint8& FromInventorySlot, const uint8& ToInventorySlot);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")							// ������ Ż�� -> ������ �̵�
		void UI_UnEquipToContainer(const uint8& FromInventorySlot, const uint8& ToInventorySlot);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")							// ������ ������ �̵�
		void UI_MoveHotbarItem(const uint8& FromSlot, const uint8& ToSlot, const bool IsDraggedFromInventory, const bool IsDraggedFromHotbar);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")							// ������ ������ �̵�
		void UI_MoveContainerItem(const uint8& FromInventorySlot, const uint8& ToInventorySlot);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")
	void UI_PerChaseItem(const uint8& InventorySlot);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")
	void UI_SellItem(const uint8& InventorySlot);
};
