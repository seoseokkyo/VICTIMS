
#pragma once

#include "CoreMinimal.h"
#include "ItemDataStructs.h"
#include "VICTIMSCharacter.h"
#include "ItemBase.generated.h"

class UInventoryComponent;
UCLASS()
class VICTIMS_API UItemBase : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TObjectPtr<UInventoryComponent> OwningInventory;
	
	UPROPERTY(VisibleAnywhere, Category = "Item")			// 아이템 정보모음
	FName ID;
	
	UPROPERTY(VisibleAnywhere, Category = "Item")
	int32 Quantity;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	EItemType ItemType;
	
	UPROPERTY(VisibleAnywhere, Category = "Item")
	EItemQuality ItemQuality;
	
	UPROPERTY(VisibleAnywhere, Category = "Item")
	FItemStatistics ItemStatistics;
	
	UPROPERTY(VisibleAnywhere, Category = "Item")
	FItemTextData TextData;
	
	UPROPERTY(VisibleAnywhere, Category = "Item")
	FItemNumericData NumericData;
	
	UPROPERTY(VisibleAnywhere, Category = "Item")
	FItemAssetData AssetData;

	bool bIsCopy;
	bool bIsPickup;

//======================================================================================
// FUNCTIONS
//======================================================================================
	UItemBase();

	void ResetItemFlags();		// 아이템 복제 후 상태 리셋

	UFUNCTION(Category = "Item")
	UItemBase* CreateItemCopy() const;
	 
	// 슬롯이 꽉찼는지 확인
	UFUNCTION(Category = "Item")
	FORCEINLINE bool IsFullItemStack() const { return Quantity == NumericData.maxStackSize; };
	
	// 개수 설정
	UFUNCTION(Category = "Item")
	void SetQuantity(const int32 NewQuantity);
	
	// 사용
	UFUNCTION(Category = "Item")
	virtual void Use(AVICTIMSCharacter* PlayerCharacter);
	
};
