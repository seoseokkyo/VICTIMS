// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BlackjackPlyaer.h"
#include "PickAceValueUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class VICTIMS_API UPickAceValueUserWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable)
	void SetBlackJackPlayer(ABlackjackPlyaer* getPlayer);

	UFUNCTION(BlueprintCallable)
	void SetAceCard(ABlackjackCard* card);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class ABlackjackPlyaer* player;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class ABlackjackCard* AceCard;

};
