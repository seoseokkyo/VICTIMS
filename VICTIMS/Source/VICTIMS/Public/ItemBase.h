
#pragma once

#include "CoreMinimal.h"
#include "ItemStructure.h"
#include "ItemBase.generated.h"


UCLASS()
class VICTIMS_API UItemBase : public UObject
{
	GENERATED_BODY()

public:

	//UPROPERTY()
	//UInventoryComponent* owningInventory;

	UPROPERTY(VisibleAnywhere, Category = "Item Data")
	int32 quantity; 

	UPROPERTY(VisibleAnywhere, Category = "Item Data")
	FName ID;                    

	UPROPERTY(VisibleAnywhere, Category = "Item Data")
	EItemType type;

	UPROPERTY(VisibleAnywhere, Category = "Item Data")
	FItemStatistics Statistics;

	UPROPERTY(VisibleAnywhere, Category = "Item Data")
	FItemTextData textData;

	UPROPERTY(VisibleAnywhere, Category = "Item Data")
	FItemNumericData numericData;

	UPROPERTY(VisibleAnywhere, Category = "Item Data")
	FItemAssetData assetData;

	//====================================================================================

	UItemBase();

	UFUNCTION(Category = "Item")
	FORCEINLINE bool IsFullItemStack() const {return quantity == numericData.maxStackCount;};  // ���� �ִ� ����������� Ȯ�� 

	UFUNCTION(Category = "Item")
	void SetQuantity(const int32 newQuantity);                                                 // ���� ���� 

	UFUNCTION(Category = "Item")
	virtual void Use(class AVICTIMSCharacter* character);                                      // ������ ��� �����Լ�


protected:
	
	bool operator == (const FName& otherID) const  // ������ ID�� ��ȿ���� Ȯ�� 
	{
		return ID == otherID;
	}

};
