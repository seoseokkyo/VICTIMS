#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemDataStructs.generated.h"

UENUM()
enum class EItemQuality : uint8					// 희귀도 
{
	common UMETA(DisplayName = "Common"),		
	better UMETA(DisplayName = "Qulity"),
	unique UMETA(DisplayName = "unique")
};

UENUM()
enum class EItemType : uint8					// 아이템 종류 대분류
{
	cloth		UMETA(DisplayName = "Cloth"),		// 옷
	weapon		UMETA(DisplayName = "Weapon"),		// 무기
	consumable  UMETA(DisplayName = "Consumable"),  // 소비
	furniture	UMETA(DisplayName = "Furniture")	// 설치(가구)
};

USTRUCT()
struct FItemStatistics							// 아이템 속성
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)							
	int32 durability; 								// 내구도
	UPROPERTY(EditAnywhere)
	int32 damageValue;								// 데미지
	UPROPERTY(EditAnywhere)
	int32 restorationAmount;						// 회복량
	UPROPERTY(EditAnywhere)
	int32 sellValue;								// 가격
};

USTRUCT()
struct FItemTextData							// 문자열 정보 
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	FText name;										// 아이템 이름
	UPROPERTY(EditAnywhere)
	FText description;								// 아이템 설명
	UPROPERTY(EditAnywhere)
	FText interactionText;							// 상호작용 여부
	UPROPERTY(EditAnywhere)
	FText usageText;								// 사용 여부 
};

USTRUCT()
struct FItemNumericData							// 아이템 보관 개수 정보
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	int32 maxStackSize;								// 최대 적재가능 개수
	UPROPERTY()
	bool bIsStackable;								// 적재보관 가능 여부 판별
};

USTRUCT()
struct FItemAssetData							// 아이템 필요 에셋 정보
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> icon;					// 아이콘 텍스쳐
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMesh> staticMesh;				// 스태틱 매시
	UPROPERTY(EditAnywhere)
	TObjectPtr<USkeletalMesh> skeletalMesh;			// 스캘레탈 매시 (옷)
};

USTRUCT()
struct FItemData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()							// enum, struct 정보 한 곳에 저장

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