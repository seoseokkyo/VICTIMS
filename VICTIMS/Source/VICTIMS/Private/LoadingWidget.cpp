// Fill out your copyright notice in the Description page of Project Settings.


#include "LoadingWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void ULoadingWidget::SetBackGroundImage(ELoadingWidgetBackGround type)
{
	InitText();

	BackGroundImage->SetBrushFromTexture(backgroundImages[(int)type], true);
}

void ULoadingWidget::OnChangedWidgetState(ESlateVisibility InVisibility)
{
	if (InVisibility == ESlateVisibility::Visible)
	{
		ToolTipString->SetText(FText::FromString(toolTipStrings[FMath::RandRange(0, toolTipStrings.Num() - 1)]));
	}
}

void ULoadingWidget::InitText()
{
	if (toolTipStrings.Num() > 0)
	{
		toolTipStrings.Reset();
	}

	toolTipStrings.AddUnique("Do Not Make Noise");
	toolTipStrings.AddUnique("Look out for others");
	toolTipStrings.AddUnique("Do Not Eat Egg");


	FString toolTipStr = toolTipStrings[FMath::RandRange(0, toolTipStrings.Num() - 1)];

	toolTipStr = toolTipStr.IsEmpty() ? toolTipStrings[FMath::RandRange(0, toolTipStrings.Num() - 1)] : toolTipStr;

	ToolTipString->SetText(FText::FromString(toolTipStr));
}

void ULoadingWidget::NativeConstruct()
{
	Super::NativeConstruct();

	OnVisibilityChanged.AddDynamic(this, &ULoadingWidget::OnChangedWidgetState);

	InitText();
}
