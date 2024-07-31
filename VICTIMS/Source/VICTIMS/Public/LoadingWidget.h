// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoadingWidget.generated.h"

/**
 * 
 */

UENUM()
enum class ELoadingWidgetBackGround
{
	Artboard_1,
	Artboard_2,
	Artboard_3,
	ELoadingWidgetBackGroundMax
};

UCLASS()
class VICTIMS_API ULoadingWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta=(BindWidget))
	class UImage* BackGroundImage;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* ToolTip;
	
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* ToolTipString;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	TArray<class UTexture2D*> backgroundImages;

	UPROPERTY()
	TArray<FString> toolTipStrings;

	UFUNCTION()
	void SetBackGroundImage(ELoadingWidgetBackGround type);

	UFUNCTION()
	void OnChangedWidgetState(ESlateVisibility InVisibility);

protected:

	UFUNCTION()
	virtual void NativeConstruct() override;

	
};
