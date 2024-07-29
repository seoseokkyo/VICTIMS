// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Victims_ContainerTab.generated.h"

/**
 * 
 */
class UVictims_ChatBox;
class UVictims_ChatMsg;

UCLASS()
class VICTIMS_API UVictims_ContainerTab : public UUserWidget
{
	GENERATED_BODY()
	
	public:

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> TabName;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UButton> TabBtn;
	

	// EChatChannelType Type
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Init Settings")
	TMap<uint8, bool> ArrM_FilteringSettingsForEachChannel;

	UPROPERTY()
	TArray<UVictims_ChatMsg*> Msges;

	UPROPERTY()
	TWeakObjectPtr<UVictims_ChatBox> MainChatBoxRef;

	UFUNCTION()
	void SetCurrentChatTabOpened();

protected:

	// UUserWidget Interface
	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;
	//~End of UUserWidget Interface
};
