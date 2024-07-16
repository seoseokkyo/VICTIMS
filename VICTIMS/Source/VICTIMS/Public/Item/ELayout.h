
#pragma once

#include "CoreMinimal.h"

UENUM()
enum class ELayout : int8
{
	Undefined = -1			UMETA(DisplayName="Undefined"),
	Inventory				UMETA(DisplayName="Inventory"),
	Equipment				UMETA(DisplayName="Equipment"),
	Container				UMETA(DisplayName="Container"),
	Shop					UMETA(DisplayName="Shop"),
	InteractText			UMETA(DisplayName="InteractText")
};

