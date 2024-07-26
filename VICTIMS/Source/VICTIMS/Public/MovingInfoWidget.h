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
	void AddPlayerName();

	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UScrollBox* MovingInfo;
	//class UListView* PlayerListView;

	void CreatePlayerButton();

	UFUNCTION(Server, Reliable)
	void ServerRPC_FindPlayers();

	UFUNCTION(Client, Reliable)
	void ClientRPC_CreateButtons(const FString& playerNames);

	UPROPERTY(meta = (BindWidget))
	class UButton* Button_GoPub;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UGoOtherPlayerButton> bp_button;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* WindowTitle;

	UFUNCTION()
	void OnClickedPubButton();

	UFUNCTION()
	void HideWidget();

	UFUNCTION()
	void VisibilityChangedEvent(ESlateVisibility InVisibility);
};
