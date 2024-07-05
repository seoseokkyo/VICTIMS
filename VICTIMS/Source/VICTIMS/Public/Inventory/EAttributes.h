#pragma once

#include "CoreMinimal.h"

UENUM()
enum class EAttributes : int8
{
	Undefined = -1	UMETA(DisplayName="Undefined"),
	
	Strength		UMETA(DisplayName="Strength"),
	Count			UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EAttributes, EAttributes::Count);
