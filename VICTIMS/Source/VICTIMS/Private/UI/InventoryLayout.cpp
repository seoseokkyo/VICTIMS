

#include "InventoryLayout.h"
#include "WindowLayout.h"
#include "Components/TextBlock.h"
#include "Internationalization/StringTableRegistry.h"
#include "Components/UniformGridpanel.h"
#include "AVICTIMSPlayerController.h"
#include "VictimsGameInstance.h"

void UInventoryLayout::NativeConstruct()
{
	Super::NativeConstruct();

	FText Text = FText::FromString("Inventory");
	Super::SetTitleToWindow(Text);

	WindowLayout = ELayout::Inventory;
}

void UInventoryLayout::UpdateGoldAmount()
{
	if (AVICTIMSPlayerController* Controller = Cast<AVICTIMSPlayerController>(GetOwningPlayer()))
	{
		uint8 LocalGoldAmount = Controller->UIGetPlayerGold();

		Gold_Amount->SetText(FText::AsNumber(LocalGoldAmount));
	}
}
