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

void UMovingInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Button_GoPub->OnClicked.AddDynamic(this, &UMovingInfoWidget::OnClickedPubButton);

	//     APlayerController* PlayerController = GetOwningPlayer();
	// 
	//     if (PlayerController)
	//     {
	//         PlayerController->bShowMouseCursor = true;
	//         FInputModeUIOnly InputMode;
	//         InputMode.SetWidgetToFocus(this->TakeWidget());  // 현재 위젯에 포커스를 맞추기
	//         InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);  // 마우스 포인터가 화면 밖으로 나가지 않게 설정
	//         PlayerController->SetInputMode(InputMode);
	//     }
}

void UMovingInfoWidget::AddPlayerName(const FString& PlayerName)
{
	if (MovingInfo)
	{
		MovingInfo->ClearChildren();

		ServerRPC_FindPlayers();
	}
}

//void UMovingInfoWidget::CreatePlayerButton(const FString& PlayerName)
//{
//    UButton* PlayerButton = NewObject<UButton>(MovingInfo);
//    PlayerButton->OnClicked.AddDynamic(this, &UMovingInfoWidget::MoveTo);
//
//    UTextBlock* ButtonText = NewObject<UTextBlock>(PlayerButton);
//    ButtonText->SetText(FText::FromString(PlayerName));
//    PlayerButton->AddChild(ButtonText);
//
//    MovingInfo->AddChild(PlayerButton);
//}

void UMovingInfoWidget::ServerRPC_FindPlayers_Implementation()
{
	for (TActorIterator<AVICTIMSPlayerController> it(GetWorld()); it; ++it)
	{
		ClientRPC_CreateButtons(*(*it)->PlayerID);
	}
}

void UMovingInfoWidget::ClientRPC_CreateButtons_Implementation(const FString& playerName)
{
	//UButton* PlayerButton = NewObject<UButton>(MovingInfo);

	//UGoOtherPlayerButton* PlayerButton = NewObject<UGoOtherPlayerButton>(MovingInfo);

	UGoOtherPlayerButton* PlayerButton = nullptr;

	if (bp_button)
	{
		PlayerButton = CreateWidget<UGoOtherPlayerButton>(MovingInfo, bp_button);
	}

	PlayerButton->playerNameTextBox->SetText(FText::FromString(playerName));

	MovingInfo->AddChild(PlayerButton);
}

void UMovingInfoWidget::MoveTo()
{

}

void UMovingInfoWidget::OnClickedPubButton()
{
	AVICTIMSPlayerController* PC = Cast<AVICTIMSPlayerController>(GetGameInstance()->GetFirstLocalPlayerController());
	//PC->bShowMouseCursor = true;

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

void UMovingInfoWidget::ClearPlayerList()
{
	if (MovingInfo)
	{
		MovingInfo->ClearChildren();
	}
}
