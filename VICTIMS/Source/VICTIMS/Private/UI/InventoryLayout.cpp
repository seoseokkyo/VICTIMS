

#include "InventoryLayout.h"
#include "WindowLayout.h"
#include "Components/TextBlock.h"
#include "Internationalization/StringTableRegistry.h"
#include "Components/UniformGridpanel.h"
#include "InventoryManagerComponent.h"
#include "AVICTIMSPlayerController.h"
#include "Components/Button.h"
#include "VictimsGameInstance.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/Sound/SoundBase.h>
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/GameplayStatics.h>

void UInventoryLayout::NativeConstruct()
{
	Super::NativeConstruct();

	FText Text = FText::FromString("Inventory");
	Super::SetTitleToWindow(Text);

	WindowLayout = ELayout::Inventory;

	
	SellButton->OnClicked.AddUniqueDynamic(this, &UInventoryLayout::OnClickSellButton);	
	SellButton->SetVisibility(ESlateVisibility::Hidden);

	DropMoneyButton->OnClicked.AddUniqueDynamic(this, &UInventoryLayout::OnClickDropMoneyButton);
}

void UInventoryLayout::UpdateGoldAmount()
{
	if (AVICTIMSPlayerController* Controller = Cast<AVICTIMSPlayerController>(GetOwningPlayer()))
	{
		int32 LocalGoldAmount = Controller->UIGetPlayerGold();

		Gold_Amount->SetText(FText::AsNumber(LocalGoldAmount));
	}
}

void UInventoryLayout::OnClickSellButton()
{
	pc = Cast<AVICTIMSPlayerController>(GetOwningPlayer());
	if (pc->InventoryManagerComponent->IsSelling == true)
	{
		pc->InventoryManagerComponent->IsSelling = false;
		SellButton->SetBackgroundColor(FLinearColor::White);
		if (click)
		{
			UGameplayStatics::PlaySound2D(GetWorld(), click);
		}
	}
	else
	{
		pc->InventoryManagerComponent->IsSelling = true;
		SellButton->SetBackgroundColor(FLinearColor::Yellow);
		if (click)
		{
			UGameplayStatics::PlaySound2D(GetWorld(), click);
		}
	}
}

void UInventoryLayout::OnClickDropMoneyButton()
{
	pc = Cast<AVICTIMSPlayerController>(GetOwningPlayer());
	if (pc)
	{
		pc->InventoryManagerComponent->Client_ShowDropMoneyLayout();
		if (click)
		{
			UGameplayStatics::PlaySound2D(GetWorld(), click);
		}
	}
}
