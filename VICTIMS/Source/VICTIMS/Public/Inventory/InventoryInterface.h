
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InventoryInterface.generated.h"

class UInventoryComponent;
UINTERFACE(MinimalAPI)
class UInventoryInterface : public UInterface
{
	GENERATED_BODY()
};


class VICTIMS_API IInventoryInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Default")							// �÷��̾� �� 
	TArray<APlayerState*> GetPlayersViewing();
//==========================================================================================================================================
	// CONTAINER
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Default")							// ������ ���� ��������
	void GetContainerProperties(FName& Namee, uint8& NumberOfRows, uint8& SlotsPerRow, bool& IsStorageContainer, uint8& InventorySize);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Default")							// ������ �κ��丮 ��������
	void GetContainerInventory(UInventoryComponent*& OutInventoryComponent);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")						// ���� �������� Ȯ��
	bool GetCanStoreItems();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")						// ���������� Ȯ�� 
	bool ContainerLooted();

//==========================================================================================================================================
	// SHOP

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")
	void GetShopProperties(FName& Namee, uint8& NumberOfRows, uint8& SlotsPerRow, bool& IsShop, uint8& InventorySize);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")
	void GetShopInventory(UInventoryComponent*& OutInventoryComponent);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")
	bool GetCanShopItems();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")
	bool ShopLooted();
	
};
