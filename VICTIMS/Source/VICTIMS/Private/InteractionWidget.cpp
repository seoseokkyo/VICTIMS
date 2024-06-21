

#include "InteractionWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "InteractionInterface.h"

void UInteractionWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	interactionProgressBar->PercentDelegate.BindUFunction(this, "UpdateInteractionProgress");
}

void UInteractionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	keyPressText->SetText(FText::FromString("Press"));
	currentInteractionDuration = 0.0f;
}

void UInteractionWidget::UpdateWidget(const FInteractableData* interactableData)
{
	switch (interactableData->interactableType)
	{
		case EInteractableType::pickUp:
		keyPressText->SetText(FText::FromString("Press"));
		interactionProgressBar->SetVisibility(ESlateVisibility::Collapsed);

		if (interactableData->quantity < 2)
		{
			quantity->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			quantity->SetText(FText::Format(NSLOCTEXT("InteractionWidget", "Quantity", "x{0}"),
				interactableData->quantity));
				quantity->SetVisibility(ESlateVisibility::Visible);
		}
		break;

		case EInteractableType::nonPlayerCharacter:
		break;

		case EInteractableType::device:
		break;

		case EInteractableType::toggle:
		break;

		case EInteractableType::container:
		break;

		default: ;

	}

	actionText->SetText(interactableData->action);
	nameText->SetText(interactableData->name);

}

float UInteractionWidget::UpdateInteractionProgress()
{
	return 0.0f;
}


