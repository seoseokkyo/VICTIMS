// Fill out your copyright notice in the Description page of Project Settings.


#include "MovingInfoWidget.h"
//#include "Components/ListView.h"
#include "Components/ScrollBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UMovingInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UMovingInfoWidget::AddPlayerName(const FString& PlayerName)
{
    if (MovingInfo)
    {
        CreatePlayerButton(PlayerName);
    }
}

void UMovingInfoWidget::CreatePlayerButton(const FString& PlayerName)
{
    UButton* PlayerButton = NewObject<UButton>(MovingInfo);
    PlayerButton->OnClicked.AddDynamic(this, &UMovingInfoWidget::MoveTo);

    UTextBlock* ButtonText = NewObject<UTextBlock>(PlayerButton);
    ButtonText->SetText(FText::FromString(PlayerName));
    PlayerButton->AddChild(ButtonText);

    MovingInfo->AddChild(PlayerButton);
}

void UMovingInfoWidget::MoveTo()
{
    UE_LOG(LogTemp, Warning, TEXT("MOVE"));
}

//void UMovingInfoWidget::ClearPlayerList()
//{
//    if (MovingInfo)
//    {
//        MovingInfo->ClearChildren();
//    }
//}
