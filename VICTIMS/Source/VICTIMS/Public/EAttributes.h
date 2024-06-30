
#pragma once

#include "CoreMinimal.h"

UENUM()
enum class EAttributes : int8									// 아이템 효과, 속성 
{
	Damage				UMETA(DisplayName = "Damage"),			// 데미지
	Heal				UMETA(DisplayName = "Heal"),			// 회복량
	Durability			UMETA(DisplayName = "Durability")		// 내구도
};