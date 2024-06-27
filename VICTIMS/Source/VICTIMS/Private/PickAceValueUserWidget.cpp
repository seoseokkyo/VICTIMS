// Fill out your copyright notice in the Description page of Project Settings.


#include "PickAceValueUserWidget.h"

void UPickAceValueUserWidget::SetBlackJackPlayer(ABlackjackPlyaer* getPlayer)
{
	player = getPlayer;
}

void UPickAceValueUserWidget::SetAceCard(ABlackjackCard* card)
{
	AceCard=card;
}
