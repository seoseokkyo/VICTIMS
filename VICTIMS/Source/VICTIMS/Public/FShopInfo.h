
#pragma once

#include "CoreMinimal.h"
#include "FShopInfo.generated.h"


USTRUCT(BlueprintType)
struct VICTIMS_API FShopInfo
{
	GENERATED_BODY();

	FShopInfo();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	FName ShopName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	uint8 NumberOfRows;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	uint8 SlotsPerRow;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	bool IsShop;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	uint8 ShopInventorySize;
};