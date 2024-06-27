#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemDataStructs.generated.h"

UENUM()
enum class EItemQuality : uint8					// ��͵� 
{
	common UMETA(DisplayName = "Common"),		
	better UMETA(DisplayName = "Qulity"),
	unique UMETA(DisplayName = "unique")
};

UENUM()
enum class EItemType : uint8					// ������ ���� ��з�
{
	cloth		UMETA(DisplayName = "Cloth"),		// ��
	weapon		UMETA(DisplayName = "Weapon"),		// ����
	consumable  UMETA(DisplayName = "Consumable"),  // �Һ�
	furniture	UMETA(DisplayName = "Furniture")	// ��ġ(����)
};

USTRUCT()
struct FItemStatistics							// ������ �Ӽ�
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)							
	int32 durability; 								// ������
	UPROPERTY(EditAnywhere)
	int32 damageValue;								// ������
	UPROPERTY(EditAnywhere)
	int32 restorationAmount;						// ȸ����
	UPROPERTY(EditAnywhere)
	int32 sellValue;								// ����
};

USTRUCT()
struct FItemTextData							// ���ڿ� ���� 
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	FText name;										// ������ �̸�
	UPROPERTY(EditAnywhere)
	FText description;								// ������ ����
	UPROPERTY(EditAnywhere)
	FText interactionText;							// ��ȣ�ۿ� ����
	UPROPERTY(EditAnywhere)
	FText usageText;								// ��� ���� 
};

USTRUCT()
struct FItemNumericData							// ������ ���� ���� ����
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	int32 maxStackSize;								// �ִ� ���簡�� ����
	UPROPERTY()
	bool bIsStackable;								// ���纸�� ���� ���� �Ǻ�
};

USTRUCT()
struct FItemAssetData							// ������ �ʿ� ���� ����
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> icon;					// ������ �ؽ���
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMesh> staticMesh;				// ����ƽ �Ž�
	UPROPERTY(EditAnywhere)
	TObjectPtr<USkeletalMesh> skeletalMesh;			// ��Ķ��Ż �Ž� (��)
};

USTRUCT()
struct FItemData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()							// enum, struct ���� �� ���� ����

	UPROPERTY(EditAnywhere, Category = "Item Data")
	FName ID;
	UPROPERTY(EditAnywhere, Category = "Item Data")
	EItemType itemType;
	UPROPERTY(EditAnywhere, Category = "Item Data")
	EItemQuality itemQuality;
	UPROPERTY(EditAnywhere, Category = "Item Data")
	FItemStatistics itemStatistics; 
	UPROPERTY(EditAnywhere, Category = "Item Data")
	FItemTextData textData;
	UPROPERTY(EditAnywhere, Category = "Item Data")
	FItemNumericData numericData;
	UPROPERTY(EditAnywhere, Category = "Item Data")
	FItemAssetData assetData;

};