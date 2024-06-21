
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemStructure.generated.h"

UENUM()
enum class EItemType : uint8												// ������ Ÿ�� ��з�
{
	equipment    UMETA(DisplayName = "Equipment"),    // ���
	consumption  UMETA(DisplayName = "Consumption"),  // �Һ�
	emplacement  UMETA(DisplayName = "Emplacement")   // ��ġ
};

USTRUCT()
struct FItemStatistics														// �Ӽ�, ���� �� 
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	int32 originPrice;     // ����

	UPROPERTY(EditAnywhere)
	int32 effectValue;     // ȿ���� (ȸ���� or ������)

	UPROPERTY(EditAnywhere)
	int32 durability;      // ������ (��� - ������, ��ġ, Į � ����) 
};

USTRUCT()
struct FItemTextData														// ������ ���� �ؽ�Ʈ ���� 
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	FText name;                  // ������ �̸�

	UPROPERTY(EditAnywhere)   
	FText description;           // ������ ���� 

	UPROPERTY(EditAnywhere)
	FText usageText;             // ���뵵 ���� 

};

USTRUCT()
struct FItemNumericData														 // ������ ���簡�� ���� 
{	
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	int32 maxStackCount;          // �ִ� ���纸�� ���� ����

	UPROPERTY(EditAnywhere)
	bool bIsStackable;            // ���纸�� ���� or �Ұ��� �Ǻ� 
};

USTRUCT()																	 // ������, StaticMesh ���� 
struct FItemAssetData 
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	UTexture2D* icon;             // ������  
	UPROPERTY(EditAnywhere)       
	UStaticMesh* mesh;            // ������ �Ž�
};

USTRUCT()
struct FItemData : public FTableRowBase                                      // ���������̺� 
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Item Data")
	FName ID;                     // ������ ���� ID

	UPROPERTY(EditAnywhere, Category = "Item Data")
	EItemType type;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	FItemStatistics Statistics;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	FItemTextData textData;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	FItemNumericData numericData;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	FItemAssetData assetData;

};
