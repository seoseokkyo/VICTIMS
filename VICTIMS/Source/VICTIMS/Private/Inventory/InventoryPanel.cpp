

#include "InventoryPanel.h"
#include "Components/GridPanel.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "InventoryComponent.h"
#include "VICTIMSCharacter.h"
#include "AVICTIMSPlayerController.h"
#include "InventoryComponent.h"
#include "InventoryItemSlot.h"
#include "ItemDragDropOperation.h"

void UInventoryPanel::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}
void UInventoryPanel::UpdateGoldAmount()
{
	if (AVICTIMSPlayerController* controller = Cast<AVICTIMSPlayerController>(GetOwningPlayer()))
	{
		uint8 LocalGoldAmount = controller->UIGetPlayerGold();
		Gold_Amount->SetText(FText::AsNumber(LocalGoldAmount));
	}
 }
