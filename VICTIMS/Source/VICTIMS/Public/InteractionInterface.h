
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractionInterface.generated.h"

class AVICTIMSCharacter;

UENUM()
enum class EInteractableType : uint8									// 상호작용 종류
{	
	PickUp					UMETA(DisplayName = "PickUp"),					// 줍기
	Store					UMETA(DisplayName = "Store"),					// 상점
	Device					UMETA(DisplayName = "Device"),					// 디바이스 (아마 맵 상호작용으로 사용?)
	Toggle					UMETA(DisplayName = "Toggle"),					// 토글
	Container				UMETA(DisplayName = "Container"),				// 보관함 
	NonPlayerCharacter		UMETA(DisplayName = "NonPlayerCharacter")		// 기타 상황
};

USTRUCT()
struct FInteractableData
{
	GENERATED_USTRUCT_BODY()

	FInteractableData() :
		InteractableType(EInteractableType::Pickup),
		Name(FText::GetEmpty()),
		Action(FText::GetEmpty()),
		Quantity(0),
		InteractionDuration(0.0f)
	{
	};
	UPROPERTY(EditInstanceOnly)
	EInteractableType InteractableType;
	UPROPERTY(EditInstanceOnly)
	FText Name;
	UPROPERTY(EditInstanceOnly)
	FText Action;
	// used only for pickups
	UPROPERTY(EditInstanceOnly)
	int8 Quantity;
	// used for things like valves, doors, etc. that require an interaction timer
	UPROPERTY(EditInstanceOnly)
	float InteractionDuration;
};
UINTERFACE(MinimalAPI)
class UInteractionInterface : public UInterface
{
	GENERATED_BODY()
};


class VICTIMS_API IInteractionInterface
{
	GENERATED_BODY()

public:
};
