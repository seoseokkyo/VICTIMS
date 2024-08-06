// Fill out your copyright notice in the Description page of Project Settings.


#include "GoOtherPlayerButton.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "AVICTIMSPlayerController.h"
#include "VICTIMSCharacter.h"
#include "MovingInfoWidget.h"


void UGoOtherPlayerButton::NativeConstruct()
{
	playerNameButton->OnClicked.AddDynamic(this, &UGoOtherPlayerButton::MoveTo);
}

void UGoOtherPlayerButton::MoveTo()
{
	//UWidget* findWidget = GetParent();

	//while (1)
	//{
	//	auto movingInfoWidget = Cast<UMovingInfoWidget>(findWidget);

	//	if (nullptr == movingInfoWidget)
	//	{
	//		findWidget = findWidget->GetParent();
	//	}
	//	else
	//	{
	//		movingInfoWidget->HideWidget();
	//		break;
	//	}
	//}

	parentMovingInfo->HideWidget();

	auto charCheck = Cast<AVICTIMSCharacter>(GetOwningPlayer()->GetPawn());

	if (charCheck)
	{
		if (charCheck->IsLocallyControlled())
		{
			charCheck->Server_GoToOtherHouse(playerNameTextBox->GetText().ToString());

			if (auto PC = Cast<AVICTIMSPlayerController>(GetOwningPlayer()))
			{
				PC->bIsShowUI = false;
				PC->DisableUIMode();
			}
		}
	}
}
