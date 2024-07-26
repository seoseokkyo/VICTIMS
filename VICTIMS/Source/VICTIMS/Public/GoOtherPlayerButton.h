// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GoOtherPlayerButton.generated.h"

/**
 * 
 */
UCLASS()
class VICTIMS_API UGoOtherPlayerButton : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct();

public:
	UPROPERTY(meta = (BindWidget))
	class UButton* playerNameButton;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* playerNameTextBox;

	UFUNCTION()
	void MoveTo();

	UPROPERTY()
	class UMovingInfoWidget* parentMovingInfo;
};
