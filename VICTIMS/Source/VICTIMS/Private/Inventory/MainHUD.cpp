

#include "MainHUD.h"
#include "MainMenu.h"
#include "InteractionWidget.h"
#include "ContainerPanel.h"
#include "HotbarPanel.h"
#include "InventoryPanel.h"
#include "ProfilePanel.h"
#include "InventoryManagerComponent.h"

AMainHUD::AMainHUD()
{
}

void AMainHUD::BeginPlay()
{
	if (MainMenuClass)
	{
		MainMenuWidget = CreateWidget<UMainMenu>(GetWorld(), MainMenuClass);
		MainMenuWidget->AddToViewport(5);
		MainMenuWidget->SetVisibility(ESlateVisibility::Visible);
	}

	if (InteractionWidgetClass)
	{
		InteractionWidget = CreateWidget<UInteractionWidget>(GetWorld(), InteractionWidgetClass);
		InteractionWidget->AddToViewport(-1);
		InteractionWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void AMainHUD::ToggleMenu(const ELayout layout)
{
	if(layout == ELayout::Inventory)
	{
		if (MainMenuWidget->Inventory)
		{
			MainMenuWidget->Inventory->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else if (layout == ELayout::Container)
	{
		if (MainMenuWidget->Container)
		{
			MainMenuWidget->Container->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else if (layout == ELayout::Equipment)
	{
		if (MainMenuWidget->Profile)
		{
			MainMenuWidget->Profile->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else if (layout == ELayout::Store)
	{
// 		if(MainMenuWidget->Store)
// 		{
// 			MainMenuWidget->Store->ToggleMenu();
// 		}
	}
}

void AMainHUD::ShowInteractionWidget() const
{
	if (InteractionWidget)
	{
		InteractionWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void AMainHUD::HideInteractionWidget() const
{
	if (InteractionWidget)
	{
		InteractionWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void AMainHUD::UpdateInteractionWidget(const FInteractableData* InteractableData) const
{
	if (InteractionWidget)
	{
		if (InteractionWidget->GetVisibility() == ESlateVisibility::Collapsed)
		{
			InteractionWidget->SetVisibility(ESlateVisibility::Visible);
		}

		InteractionWidget->UpdateWidget(InteractableData);
	}
}