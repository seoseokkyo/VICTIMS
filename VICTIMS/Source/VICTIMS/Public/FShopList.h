
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "FShopList.generated.h"

USTRUCT(BlueprintType)
struct VICTIMS_API FShopList : public FTableRowBase
{
	GENERATED_BODY();

	FShopList();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Amount; 
};