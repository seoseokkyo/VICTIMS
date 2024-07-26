// Fill out your copyright notice in the Description page of Project Settings.


#include "Victims_FloatingWidget.h"

#include "Animation/WidgetAnimation.h"
#include "Blueprint/GameViewportSubsystem.h"
#include "VictimsChatManager.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"

void UVictims_FloatingWidget::NativeConstruct()
{
	Super::NativeConstruct();

	PlayAnimationForward(WidgetAppearAnim);
	ETB_WhisperInput->SetKeyboardFocus();

	TB_WidgetTitle->SetText(WidgetTitle);
	TB_Description->SetText(WidgetDescription);

	TB_Accept->SetText(AcceptBtnText);
	if (TB_Cancel)
	{
		TB_Cancel->SetText(CancelBtnText);
	}

	CheckBtn->OnClicked.AddUniqueDynamic(this, &ThisClass::OnClickedAcceptBtnFunc);

	SB_Cancel->SetVisibility(bHasCancelBtn ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	ETB_WhisperInput->SetVisibility(bNeedEditableTextBox ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);

	if (CancelBtn)
	{
		CancelBtn->OnClicked.AddUniqueDynamic(this, &ThisClass::OnClickedCancelBtnFunc);
	}
}

void UVictims_FloatingWidget::AfterDisappearEvent()
{
	// Remove from parent
	if (this)
	{
		if (!HasAnyFlags(RF_BeginDestroyed))
		{
			if (bIsManagedByGameViewportSubsystem)
			{
				if (UGameViewportSubsystem * Subsystem{ UGameViewportSubsystem::Get(GetWorld()) })
				{
					Subsystem->RemoveWidget(this);
				}
			}
			else if (UPanelWidget * CurrentParent{ GetParent() })
			{
				CurrentParent->RemoveChild(this);
			}
			else
			{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
				if (GetCachedWidget().IsValid() && GetCachedWidget()->GetParentWidget().IsValid() && !IsDesignTime())
				{
					// FText WarningMessage = FText::Format(LOCTEXT("RemoveFromParentWithNoParent", "UWidget::RemoveFromParent() called on '{0}' which has no UMG parent (if it was added directly to a native Slate widget via TakeWidget() then it must be removed explicitly rather than via RemoveFromParent())"), FText::AsCultureInvariant(GetPathName()));
					// @todo: nickd - we need to switch this back to a warning in engine, but info for games
					// FMessageLog("PIE").Info(WarningMessage);
				}
#endif
			}
		}
	}
}

void UVictims_FloatingWidget::OnClickedAcceptBtnFunc()
{
	AcceptBtnContentLogic();

	BeforeRemoveFromParent();
}

void UVictims_FloatingWidget::AcceptBtnContentLogic()
{
	OnClickedAcceptBtn.ExecuteIfBound(GetOwningPlayer());
}

void UVictims_FloatingWidget::OnClickedCancelBtnFunc()
{
	CancelBtnContentLogic();

	BeforeRemoveFromParent();
}

void UVictims_FloatingWidget::CancelBtnContentLogic()
{
	OnClickedCancelBtn.ExecuteIfBound(GetOwningPlayer());
}

void UVictims_FloatingWidget::BeforeRemoveFromParent()
{
	PlayAnimationReverse(WidgetAppearAnim);
	FWidgetAnimationDynamicEvent AfterDisappearDelegate;
	AfterDisappearDelegate.BindUFunction(this, FName("AfterDisappearEvent"));
	BindToAnimationFinished(WidgetAppearAnim, AfterDisappearDelegate);
}

void UVictims_FloatingWidget::RemoveFromParent()
{
	Super::RemoveFromParent();
}

FReply UVictims_FloatingWidget::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (const FKey PressedKey{ InKeyEvent.GetKey() }; PressedKey == EKeys::Enter)
	{
		// UE_LOG(LogTemp, Error, L"Whisper Enter");
		if (ETB_WhisperInput->HasKeyboardFocus())
		{
			AcceptBtnContentLogic();

			BeforeRemoveFromParent();

			return FReply::Handled();
		}
	}
	return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
}
