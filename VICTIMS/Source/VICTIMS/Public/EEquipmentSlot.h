
#pragma once

#include "CoreMinimal.h"

UENUM()
enum class EEquipmentSlot : int8								// 장비 분류
{
	Undefined = -1			UMETA(DisplayName = "Undefined"),
	Weapon				    UMETA(DisplayName = "Weapon"),
	Head					UMETA(DisplayName = "Head"),
	Top						UMETA(DisplayName = "Top"),
	Bottom					UMETA(DisplayName = "Bottom"),
	Feet					UMETA(DisplayName = "Feet"),
	Count					UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EEquipmentSlot, EEquipmentSlot::Count);