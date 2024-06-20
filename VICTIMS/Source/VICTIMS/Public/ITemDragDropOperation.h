
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "VICTIMSCharacter.h"
#include "ITemDragDropOperation.generated.h"


class UItems;
class UInventoryComponent;

UCLASS()
class VICTIMS_API UITemDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY()
	UItems* sourceItem;

	UPROPERTY()
	UInventoryComponent* sourceInventory;
};
