// Fill out your copyright notice in the Description page of Project Settings.


#include "BlajackMainWidget.h"
#include <../../../../../../../Source/Runtime/UMG/Public/Components/TextBlock.h>

void UBlajackMainWidget::NativeConstruct()
{
	Super::NativeConstruct();

	playerScoreDelegate.BindUFunction(this, FName(TEXT("SetPlayerScore")));
	dealerScoreDelegate.BindUFunction(this, FName(TEXT("SetDealerScore")));
}

void UBlajackMainWidget::SetPlayer(ABlackjackPlyaer* InputPlayer)
{
	blackjackPlayer =  InputPlayer;
}

void UBlajackMainWidget::SetDealer(ABlackjackTable* InputDealer)
{
	dealer=InputDealer;
}

void UBlajackMainWidget::SetPlayerScore(int32 value)
{
	PlayerScore->SetText(FText::FromString(FString::FromInt(value)));
}

void UBlajackMainWidget::SetDealerScore(int32 value)
{
	DealerScore->SetText(FText::FromString(FString::FromInt(value)));
}