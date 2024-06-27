
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractionInterface.generated.h"

class AVICTIMSCharacter;

UENUM()
enum class EInteractableType : uint8									// ��ȣ�ۿ� ����
{	
	PickUp					UMETA(DisplayName = "PickUp"),					// �ݱ�
	Store					UMETA(DisplayName = "Store"),					// ����
	Device					UMETA(DisplayName = "Device"),					// ����̽� (�Ƹ� �� ��ȣ�ۿ����� ���?)
	Toggle					UMETA(DisplayName = "Toggle"),					// ���
	Container				UMETA(DisplayName = "Container"),				// ������ 
	NonPlayerCharacter		UMETA(DisplayName = "NonPlayerCharacter")		// ��Ÿ ��Ȳ
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
