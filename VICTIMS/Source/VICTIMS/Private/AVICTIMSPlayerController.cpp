

#include "AVICTIMSPlayerController.h"
#include "InventoryComponent.h"
#include "ItemDataStructs.h"

AVICTIMSPlayerController::AVICTIMSPlayerController()
{
	inventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	inventoryComponent->SetIsReplicated(true);
}

void AVICTIMSPlayerController::UseInventoryItem_Implementation(const TObjectPtr<UItemBase> _item)
{
	inventoryComponent->UseItem(_item);
}
