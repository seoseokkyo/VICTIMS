

#include "ContainerLayout.h"
#include "VictimsGameInstance.h"
#include "AVICTIMSPlayerController.h"
#include "LootActor.h"
#include "InventoryManagerComponent.h"

void UContainerLayout::NativeConstruct()
{
	Super::NativeConstruct();
	OnNativeVisibilityChanged.AddUObject(this, &UContainerLayout::SetTextToContainer);
	WindowLayout = ELayout::Container;
}

void UContainerLayout::SetTextToContainer(ESlateVisibility InVisibility)
{
	if (InVisibility == ESlateVisibility::Visible)
	{
		FText Text;
		if (IsStorageContainer)
		{
			Text = FText::FromString(TEXT("Container"));
		}
		else 
		{
			Text = FText::FromString(TEXT("Dummy"));
		}
		Super::SetTitleToWindow(Text);
	}
}

void UContainerLayout::OnButtonQuitClicked()
{
	if (IsValid(PlayerController))
	{
		PlayerController->InventoryManagerComponent->Server_CloseContainer();
		PlayerController->SetInputDependingFromVisibleWidgets();
	}
}

