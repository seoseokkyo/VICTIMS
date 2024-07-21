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

//	UFUNCTION(BlueprintCallable)
//	void ClearPlayerList();
};
