// Fill out your copyright notice in the Description page of Project Settings.


#include "GoOtherPlayerButton.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "AVICTIMSPlayerController.h"
#include "VICTIMSCharacter.h"


void UGoOtherPlayerButton::NativeConstruct()
{
	playerNameButton->OnClicked.AddDynamic(this, &UGoOtherPlayerButton::MoveTo);
}

void UGoOtherPlayerButton::MoveTo()
{
	auto charCheck = Cast<AVICTIMSCharacter>(GetOwningPlayer()->GetPawn());

	if (charCheck)
	{
		if (charCheck->IsLocallyControlled())
		{
			charCheck->Server_GoToOtherHouse(playerNameTextBox->GetText().ToString());
		}
	}
}
