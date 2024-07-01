
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HUDInterface.generated.h"


class UInventoryComponent;
UINTERFACE(MinimalAPI)
class UHUDInterface : public UInterface
{
	GENERATED_BODY()
};


class VICTIMS_API IHUDInterface
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")
	void UI_MoveInventoryItem(const uint8& FromInventorySlot, const uint8& ToInventorySlot);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")
	void UI_DropInventoryItem(const uint8& InventorySlot);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")
	void UI_EquipInventoryItem(const uint8& FromInventorySlot, const uint8& ToInventorySlot);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")
	void UI_UnEquipInventoryItem(const uint8& FromInventorySlot, const uint8& ToInventorySlot);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")
	void UI_UseInventoryItem(const uint8& InventorySlot);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")
	void UI_UseContainerItem(const uint8& InventorySlot);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")
	void UI_UseHotbarItem(const uint8& InventorySlot);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")
	void UI_TakeContainerItem(const uint8& FromContainerSlot, const uint8& ToInventorySlot);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")
	void UI_DepositContainerItem(const uint8& FromInventorySlot, const uint8& ToInventorySlot);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")
	void UI_EquipFromContainer(const uint8& FromInventorySlot, const uint8& ToInventorySlot);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")
	void UI_UnEquipToContainer(const uint8& FromInventorySlot, const uint8& ToInventorySlot);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")
	void UI_MoveHotbarItem(const uint8& FromSlot, const uint8& ToSlot, const bool IsDraggedFromInventory, const bool IsDraggedFromHotbar);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")
	void UI_MoveContainerItem(const uint8& FromInventorySlot, const uint8& ToInventorySlot);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")
	void GetContainerProperties(FName& Namee, uint8& NumberOfRows, uint8& SlotsPerRow, bool& IsStorageContainer, uint8& InventorySize);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")
	void GetContainerInventory(UInventoryComponent*& OutInventoryComponent);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")
	TArray<APlayerState*> GetPlayersViewing();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")
	bool GetCanStoreItems();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")
	bool ContainerLooted();

};
