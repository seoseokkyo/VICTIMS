// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KillWidget.generated.h"

/**
 * 
 */
UCLASS()
class VICTIMS_API UKillWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, Transient, Category = "UI Animation", meta = (BindWidgetAnim))
	class UWidgetAnimation* KillAnimation;

	
};
