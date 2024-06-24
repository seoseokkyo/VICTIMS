
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemStructure.generated.h"

UENUM()
enum class EItemType : uint8												// 아이템 타입 대분류
{
	equipment    UMETA(DisplayName = "Equipment"),    // 장비
	consumption  UMETA(DisplayName = "Consumption"),  // 소비
	emplacement  UMETA(DisplayName = "Emplacement")   // 설치
};

USTRUCT()
struct FItemStatistics														// 속성, 스탯 값 
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	int32 originPrice;     // 가격

	UPROPERTY(EditAnywhere)
	int32 effectValue;     // 효과값 (회복량 or 데미지)

	UPROPERTY(EditAnywhere)
	int32 durability;      // 내구도 (장비 - 몽둥이, 망치, 칼 등에 적용) 
};

USTRUCT()
struct FItemTextData														// 아이템 정보 텍스트 관련 
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	FText name;                  // 아이템 이름

	UPROPERTY(EditAnywhere)   
	FText description;           // 아이템 설명 

	UPROPERTY(EditAnywhere)
	FText usageText;             // 사용용도 설명 

};

USTRUCT()
struct FItemNumericData														 // 아이템 적재가능 개수 
{	
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	int32 maxStackCount;          // 최대 적재보관 가능 개수

	UPROPERTY(EditAnywhere)
	bool bIsStackable;            // 적재보관 가능 or 불가능 판별 
};

USTRUCT()																	 // 아이콘, StaticMesh 설정 
struct FItemAssetData 
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	UTexture2D* icon;             // 아이콘  
	UPROPERTY(EditAnywhere)       
	UStaticMesh* mesh;            // 아이템 매시
};

USTRUCT()
struct FItemData : public FTableRowBase                                      // 데이터테이블 
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Item Data")
	FName ID;                     // 아이템 고유 ID

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
