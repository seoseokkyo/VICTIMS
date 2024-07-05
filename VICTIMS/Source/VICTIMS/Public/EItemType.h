

#pragma once

#include "CoreMinimal.h"


UENUM(BlueprintType)
enum class EItemType : uint8
{
	Undefined = 0			UMETA(DisplayName="Undefined"),
	Furniture				UMETA(DisplayName="Furniture"),
	Equipment				UMETA(DisplayName="Equipment"),
	Consumable				UMETA(DisplayName="Consumable"),
	Currency				UMETA(DisplayName="Currency")
};