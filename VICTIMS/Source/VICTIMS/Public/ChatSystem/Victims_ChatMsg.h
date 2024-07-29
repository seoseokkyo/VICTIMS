// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"
#include "Victims_ChatBox.h"

#include "Victims_ChatMsg.generated.h"

/**
 * 
 */

UCLASS()
class VICTIMS_API UVictims_ChatMsg : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> Msg;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn), Category = "Message Info")
	FVictims_MsgInfo MsgInfo;
	
protected:

	// UUserWidget Interface
	virtual void NativeConstruct() override;
	//~End of UUserWidget Interface
	
};
