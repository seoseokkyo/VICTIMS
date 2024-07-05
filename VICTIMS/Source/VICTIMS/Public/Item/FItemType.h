#pragma once

#include "CoreMinimal.h"
#include "FItemType.generated.h"

USTRUCT(BlueprintType)
struct FItemType
{
	GENERATED_BODY();

	static FName Undefined;
	static FName Furniture;
	static FName Equipment;
	static FName Consumable;
	static FName Currency;
};