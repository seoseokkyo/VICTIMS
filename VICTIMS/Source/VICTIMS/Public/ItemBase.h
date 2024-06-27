
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
	
	UPROPERTY(VisibleAnywhere, Category = "Item")			// ������ ��������
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

	void ResetItemFlags();		// ������ ���� �� ���� ����

	UFUNCTION(Category = "Item")
	UItemBase* CreateItemCopy() const;
	 
	// ������ ��á���� Ȯ��
	UFUNCTION(Category = "Item")
	FORCEINLINE bool IsFullItemStack() const { return Quantity == NumericData.maxStackSize; };
	
	// ���� ����
	UFUNCTION(Category = "Item")
	void SetQuantity(const int32 NewQuantity);
	
	// ���
	UFUNCTION(Category = "Item")
	virtual void Use(AVICTIMSCharacter* PlayerCharacter);
	
};
