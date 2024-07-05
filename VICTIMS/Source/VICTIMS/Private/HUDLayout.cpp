

#include "HUDLayout.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "AVICTIMSPlayerController.h"
#include "FWidgetsLayoutBP.h"
#include "Engine/DataTable.h"

void UHUDLayout::NativeConstruct()
{
	Btn_Inventory->OnReleased.AddUniqueDynamic(this, &UHUDLayout::OnBtnUIClicked);
	Btn_Profile->OnReleased.AddUniqueDynamic(this, &UHUDLayout::OnBtnUIClicked);
}


void UHUDLayout::OnBtnUIClicked()
{
	if (AVICTIMSPlayerController* Controller = Cast<AVICTIMSPlayerController>(GetOwningPlayer()))
	{
		if (Btn_Inventory->IsHovered())
		{
			Controller->ToggleInventory();
		}
		else if (Btn_Profile->IsHovered())
		{
			Controller->ToggleProfile();
		}
	};
}
