// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BlackjackJoinWidget.generated.h"

/**
 * 
 */
UCLASS()
class VICTIMS_API UBlackjackJoinWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* OwnerPlayer;

	UFUNCTION(BlueprintCallable)
	void SetOwnerActor(AActor*Owner);
	
};
