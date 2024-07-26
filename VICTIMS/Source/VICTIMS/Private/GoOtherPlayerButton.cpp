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
	UWidget* findWidget = nullptr;

	while (1)
	{
		findWidget = Cast<UMovingInfoWidget>(GetParent());

		if (nullptr == findWidget)
		{
			findWidget = findWidget->GetParent();
		}
		else
		{
			Cast<UMovingInfoWidget>(findWidget)->HideWidget();
			break;
		}
	}

	auto charCheck = Cast<AVICTIMSCharacter>(GetOwningPlayer()->GetPawn());

	if (charCheck)
	{
		if (charCheck->IsLocallyControlled())
		{
			charCheck->Server_GoToOtherHouse(playerNameTextBox->GetText().ToString());
		}
	}
}
