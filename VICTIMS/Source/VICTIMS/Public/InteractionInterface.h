
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractionInterface.generated.h"

class AVICTIMSCharacter;

UENUM()
enum class EInteractableType : uint8
{
	pickUp				UMETA(DisplayName = "PickUp"),
	nonPlayerCharacter	UMETA(DisplayName = "NonPlayerCharacter"),
	device				UMETA(DisplayName = "Device"),
	toggle				UMETA(DisplayName = "Toggle"),
	container			UMETA(DisplayName = "Container")
};

USTRUCT()
struct FInteractableData
{
	GENERATED_USTRUCT_BODY()

	FInteractableData() :
	interactableType(EInteractableType::pickUp),
	name(FText::GetEmpty()),
	action(FText::GetEmpty()),
	quantity(0),
	interactionDuration(0.0f)
	{

	};

	UPROPERTY(EditInstanceOnly)
	EInteractableType interactableType;

	UPROPERTY(EditInstanceOnly)
	FText name;

	UPROPERTY(EditInstanceOnly)
	FText action;

	UPROPERTY(EditInstanceOnly)
	int8 quantity;

	UPROPERTY(EditInstanceOnly)
	float interactionDuration; // 상호작용 걸리는 시간

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
	virtual void BeginFocus();
	virtual void EndFocus();
	virtual void BeginInteract();
	virtual void EndInteract();
	virtual void Interact(class AVICTIMSCharacter* playerCharacter);

	FInteractableData interactableData;
};
