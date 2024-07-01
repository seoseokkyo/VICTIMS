

#include "ContainerPanel.h"
#include "InventoryManagerComponent.h"
#include "AVICTIMSPlayerController.h"

void UContainerPanel::NativeConstruct()
{
	Controller = Cast<AVICTIMSPlayerController>(GetOwningPlayer());
}

void UContainerPanel::SetTextToContainer(ESlateVisibility inVisibility)
{
}


