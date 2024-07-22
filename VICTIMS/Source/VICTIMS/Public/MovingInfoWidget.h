// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MovingInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class VICTIMS_API UMovingInfoWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Player List")
	void AddPlayerName(const FString& PlayerName);

	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UScrollBox* MovingInfo;
	//class UListView* PlayerListView;

	void CreatePlayerButton(const FString& PlayerName);

	UFUNCTION()
	void MoveTo();

	UPROPERTY(meta = (BindWidget))
	class UButton* Button_GoPub;

	UFUNCTION()
	void OnClickedPubButton();

	FTimerHandle HideWidgetTimerHandle;

	UFUNCTION()
	void HideWidget();

	UFUNCTION(BlueprintCallable)
	void ClearPlayerList();
};
