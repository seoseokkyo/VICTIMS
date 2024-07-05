
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
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Default")							// 보관함 정보 가져오기
	void GetContainerProperties(FName& Namee, uint8& NumberOfRows, uint8& SlotsPerRow, bool& IsStorageContainer, uint8& InventorySize);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Default")							// 보관함 인벤토리 가져오기
	void GetContainerInventory(UInventoryComponent*& OutInventoryComponent);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Default")							// 플레이어 뷰 
	TArray<APlayerState*> GetPlayersViewing();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")						// 보관 가능한지 확인
	bool GetCanStoreItems();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")						// 보관함인지 확인 
	bool ContainerLooted();
};
