
#pragma once

#include "CoreMinimal.h"
#include "ItemBase.h"
#include "Items.generated.h"

class UInventoryComponent;
class UTexture2D;
class UStaticMesh;

UENUM()
enum class EItemNumber : uint8
{
	MaxEItemNumber
};

UENUM()
enum class EItemTYPE : uint8
{
	Stuff,
	Equipable,
	Consumable_Direct,
	Consumable_Indirect,
	Buildable,
	EItemTYPE
};

UENUM()
enum class EItemMatter : uint8
{
	Wood,
	Stone,
	Metal,
	EItemMaterialType
};

//USTRUCT(BlueprintType)
//struct FItemBaseUsingUIData : public FTableRowBase
//{
//	GENERATED_BODY()
//
//
//};

USTRUCT(BlueprintType)
struct FItemBaseData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FText                   name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EItemNumber				itemNumber;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EItemTYPE				itemType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EItemMatter				itemMatterType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D*             itemTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh*            itemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString					itemDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32					itemOriginPrice;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32					itemDurability;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32					itemCustomValue1; // 회복량 등으로 사용 가능

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32					itemCustomValue2;

	//UPROPERTY(BlueprintReadWrite)
	//FItemBaseUsingUIData	usingUIData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bStackable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxStackCount;
};

UCLASS()
class VICTIMS_API UItems : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category = "Item")
	FName ID; 

	UPROPERTY(VisibleAnywhere, Category = "Item")
	int32 Quantity; // 수량

	// 인벤토리 컴포넌트 
	UPROPERTY(VisibleAnywhere, Category = "Item")
	UInventoryComponent* owningInventory; 

	FItemBaseData itemData;
	UFUNCTION(Category = "Item")
	FORCEINLINE bool IsFullItemStack() const { return Quantity == itemData.MaxStackCount;};// 전체 항목 스텍, 수량이 최대 스택 크기와 같은지

	UFUNCTION(Category = "Item")
	void SetQuantity(const int32 NewQuantity); // 수량 같은지 체크

protected:

	bool operator == (const FName& otherID) const // TArray 비교 
	{
		return this->ID == otherID;
	}
};
