

#include "Shop_Slo.h"
#include "FItemType.h"
#include "VictimsGameInstance.h"
#include "AVICTIMSPlayerController.h"
#include "ShopActor.h"
#include "Components/TextBlock.h"
#include "InventoryComponent.h"
#include "Components/UniformGridPanel.h"
#include "Components/ScrollBox.h"
#include "InventoryManagerComponent.h"
#include "Components/Border.h"
#include "Components/Button.h"

void UShop_Slo::NativeConstruct()
{
	Super::NativeConstruct();

	NotificationText->SetText(FText::FromString(TEXT("Don't Have Enough Money!")));
	NotificationBorder->SetVisibility(ESlateVisibility::Collapsed);
	NotificationText->SetVisibility(ESlateVisibility::Collapsed);
}


void UShop_Slo::SetPriceValue(const FItemStructure& Item)
{
	int32 Price = SlotStructure.ItemStructure.PriceValue;
	PriceValue->SetText(FText::AsNumber(Price));
}

void UShop_Slo::ShowNotification()
{
	FTimerHandle Timer;
	NotificationBorder->SetVisibility(ESlateVisibility::Visible);
	NotificationText->SetVisibility(ESlateVisibility::Visible);
	GetWorld()->GetTimerManager().ClearTimer(Timer);
	GetWorld()->GetTimerManager().SetTimer(Timer, [&]() {
		NotificationBorder->SetVisibility(ESlateVisibility::Collapsed);
		NotificationText->SetVisibility(ESlateVisibility::Collapsed);
		}, 1.0f, false);
}