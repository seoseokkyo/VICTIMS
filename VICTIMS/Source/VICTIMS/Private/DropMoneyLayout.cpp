

#include "DropMoneyLayout.h"
#include "AVICTIMSPlayerController.h"
#include "Components/Border.h"
#include "Components/EditableText.h"
#include "InventoryManagerComponent.h"
#include "Components/Button.h"


void UDropMoneyLayout::NativeConstruct()
{
	Super::NativeConstruct();

	DropButton->OnClicked.AddUniqueDynamic(this, &UDropMoneyLayout::OnClickedDropButton);

	NotificationBorder->SetVisibility(ESlateVisibility::Collapsed);
}

void UDropMoneyLayout::OnClickedDropButton()
{
	AVICTIMSPlayerController* PC = Cast<AVICTIMSPlayerController>(GetOwningPlayer());
	if(PC)
	{
		PC->InventoryManagerComponent->DropMoney();
	}
}
