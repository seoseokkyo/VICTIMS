

#include "AVICTIMSPlayerController.h"
#include "InventoryComponent.h"
#include "ItemDataStructs.h"

AVICTIMSPlayerController::AVICTIMSPlayerController()
{
	inventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	inventoryComponent->SetIsReplicated(true);
}

