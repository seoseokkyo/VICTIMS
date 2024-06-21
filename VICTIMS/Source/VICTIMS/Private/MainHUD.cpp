

#include "MainHUD.h"
#include "MainMenu.h"
#include "InteractionWidget.h"

AMainHUD::AMainHUD()
{

}

void AMainHUD::BeginPlay()
{
	Super::BeginPlay();

	if (mainMenu_wbp)
	{
		mainMenu = CreateWidget<UMainMenu>(GetWorld(), mainMenu_wbp);
		mainMenu->AddToViewport(5);
		mainMenu->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (interaction_wbp)
	{
		interactionWidget = CreateWidget<UInteractionWidget>(GetWorld(), interaction_wbp);
		interactionWidget->AddToViewport(-1);
		interactionWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}
void AMainHUD::DisplayMenu()
{
	if (mainMenu)
	{
		bIsMenuVisible = true;
		mainMenu->SetVisibility(ESlateVisibility::Visible);
	}
}

void AMainHUD::HideMenu()
{
	if (mainMenu)
	{
		bIsMenuVisible = false;
		mainMenu->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void AMainHUD::ShowInteractionWidget() const
{
	if (interactionWidget)
	{
		interactionWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void AMainHUD::HideInteractionWidget() const
{
	if (interactionWidget)
	{
		interactionWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void AMainHUD::UpdateInteractionWidget(const FInteractableData* interactableData) const
{
	if (interactionWidget)
	{
		if (interactionWidget->GetVisibility() == ESlateVisibility::Collapsed)
		{
			interactionWidget->SetVisibility(ESlateVisibility::Visible);
		}
		
 			interactionWidget->UpdateWidget(interactableData);
	}
}

