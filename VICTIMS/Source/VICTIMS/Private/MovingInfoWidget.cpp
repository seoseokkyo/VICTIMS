// Fill out your copyright notice in the Description page of Project Settings.


#include "MovingInfoWidget.h"
//#include "Components/ListView.h"
#include "Components/ScrollBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "AVICTIMSPlayerController.h"
#include "Player/VICTIMSCharacter.h"

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
//         InputMode.SetWidgetToFocus(this->TakeWidget());  // ���� ������ ��Ŀ���� ���߱�
//         InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);  // ���콺 �����Ͱ� ȭ�� ������ ������ �ʰ� ����
//         PlayerController->SetInputMode(InputMode);
//     }
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
