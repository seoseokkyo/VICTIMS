

#include "ShopLayout.h"
#include "VictimsGameInstance.h"
#include "AVICTIMSPlayerController.h"
#include "ShopActor.h"
#include "Components/TextBlock.h"
#include "InventoryComponent.h"
#include "FItemType.h"
#include "Components/UniformGridPanel.h"
#include "Components/ScrollBox.h"
#include "InventoryManagerComponent.h"
#include "Components/Border.h"
#include "Components/Button.h"

void UShopLayout::NativeConstruct()
{
	Super::NativeConstruct();

	WindowLayout = ELayout::Shop;
}
