// Fill out your copyright notice in the Description page of Project Settings.


#include "MovingInfoWidget.h"
//#include "Components/ListView.h"
#include "Components/ScrollBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "AVICTIMSPlayerController.h"
#include "Player/VICTIMSCharacter.h"
#include "AVICTIMSPlayerController.h"
#include <../../../../../../../Source/Runtime/Engine/Public/EngineUtils.h>
#include "GoOtherPlayerButton.h"
#include "VICTIMSGameMode.h"
#include "Kismet/KismetSystemLibrary.h"

void UMovingInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	OnVisibilityChanged.AddDynamic(this, &UMovingInfoWidget::VisibilityChangedEvent);
	Button_GoPub->OnClicked.AddDynamic(this, &UMovingInfoWidget::OnClickedPubButton);
	WindowTitle->SetText(FText::FromString("select place to visit"));
}

void UMovingInfoWidget::AddPlayerName()
{
	auto myPlayer = GetOwningPlayer();
	if (myPlayer && myPlayer->IsLocalController())
	{
		if (MovingInfo)
		{
			MovingInfo->ClearChildren();
		}

		if (auto victimPlayer = Cast<AVICTIMSPlayerController>(myPlayer))
		{
			victimPlayer->ServerRPC_UpdatePlayerList();

			victimPlayer->OnChangedPlayerList.BindLambda([&, victimPlayer](TArray<FString> arg) {

				for (auto playerName : arg)
				{
					if (playerName.IsEmpty() == false)
					{
						if (bp_button)
						{
							UGoOtherPlayerButton* PlayerButton = nullptr;

							PlayerButton = CreateWidget<UGoOtherPlayerButton>(MovingInfo, bp_button);
							PlayerButton->playerNameTextBox->SetText(FText::FromString(playerName));
							MovingInfo->AddChild(PlayerButton);

							PlayerButton->parentMovingInfo = this;
						}
					}
				}

				victimPlayer->OnChangedPlayerList.Unbind();
			});
		}
	}
}

void UMovingInfoWidget::ServerRPC_FindPlayers_Implementation()
{
	auto gameMode = GetWorld()->GetAuthGameMode<AVICTIMSGameMode>();

	if (gameMode)
	{
		auto players = gameMode->GetPlayers();

		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("players Num : %d"), players.Num()));

		for (auto player : players)
		{
			UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("player Ptr : %p"), player));
			ClientRPC_CreateButtons(player->playerName);
		}
	}
}

void UMovingInfoWidget::ClientRPC_CreateButtons_Implementation(const FString& playerName)
{
	UGoOtherPlayerButton* PlayerButton = nullptr;

	if (bp_button)
	{
		PlayerButton = CreateWidget<UGoOtherPlayerButton>(MovingInfo, bp_button);
		PlayerButton->playerNameTextBox->SetText(FText::FromString(playerName));
		MovingInfo->AddChild(PlayerButton);
	}
}

void UMovingInfoWidget::OnClickedPubButton()
{
	AVICTIMSPlayerController* PC = Cast<AVICTIMSPlayerController>(GetOwningPlayer());

	if (PC)
	{
		AVICTIMSCharacter* Character = Cast<AVICTIMSCharacter>(PC->GetPawn());
		if (Character)
		{
			Character->SetActorLocation(FVector(1850, 821, 169));
		}
	}

	HideWidget();
}

void UMovingInfoWidget::HideWidget()
{
	SetVisibility(ESlateVisibility::Hidden);

	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->bShowMouseCursor = false;
		PC->SetInputMode(FInputModeGameOnly());
	}
}

void UMovingInfoWidget::VisibilityChangedEvent(ESlateVisibility InVisibility)
{
	AVICTIMSPlayerController* PC = Cast<AVICTIMSPlayerController>(GetGameInstance()->GetFirstLocalPlayerController());

	//if (GetOwningLocalPlayer() == nullptr)
	//{
	//	SetOwningLocalPlayer(PC->GetLocalPlayer());
	//	SetOwningPlayer(PC);
	//}

	//if (GetOwningPlayer() == nullptr)
	//{
	//	SetOwningLocalPlayer(PC->GetLocalPlayer());
	//	SetOwningPlayer(PC);
	//}

	//if (GetOwningPlayerPawn() == nullptr)
	//{
	//	SetOwningLocalPlayer(PC->GetLocalPlayer());
	//	SetOwningPlayer(PC);
	//}

	//if (GetOwningPlayerState() == nullptr)
	//{
	//	SetOwningLocalPlayer(PC->GetLocalPlayer());
	//	SetOwningPlayer(PC);
	//}

	if (InVisibility == ESlateVisibility::Visible)
	{
		AddPlayerName();
	}
}
