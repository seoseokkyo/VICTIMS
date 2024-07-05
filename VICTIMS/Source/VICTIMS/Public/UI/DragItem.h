
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "FSlotStructure.h"
#include "DragItem.generated.h"


UCLASS()
class VICTIMS_API UDragItem : public UDragDropOperation
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlotStructure DraggedSlotInformation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 DraggedSlotIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsDraggedFromInventory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsDraggedFromContainer;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsDraggedFromHotbar;
	
};
