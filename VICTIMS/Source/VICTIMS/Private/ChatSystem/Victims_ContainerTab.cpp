// Fill out your copyright notice in the Description page of Project Settings.

#include "Victims_ContainerTab.h"
#include <Components/Button.h>
#include <Components/PanelWidget.h>
#include <Framework/Application/SlateApplication.h>
#include "Victims_ChatBox.h"


void UVictims_ContainerTab::SetCurrentChatTabOpened()
{
	if (MainChatBoxRef.IsValid())
	{
		if (MainChatBoxRef->CurrentOpenedTab)
		{
			for (UWidget* Elem : static_cast<UPanelWidget*>(MainChatBoxRef->HB_Tabs)->GetAllChildren())
			{
				if (const UVictims_ContainerTab * IsContainerTab{ Cast<UVictims_ContainerTab>(Elem) })
				{
					IsContainerTab->TabBtn->SetBackgroundColor(FLinearColor::White);
				}
			}
		}
		MainChatBoxRef->CurrentOpenedTab = this;
		MainChatBoxRef->UpdateCurrentChatBoxMessages(this);
		TabBtn->SetBackgroundColor(FLinearColor(1.f, 1.f, 1.f, 0.75f));
		FSlateApplication::Get().SetAllUserFocusToGameViewport();
		MainChatBoxRef->ToggleChatBox(true);
	}
}

void UVictims_ContainerTab::NativeConstruct()
{
	Super::NativeConstruct();
}

void UVictims_ContainerTab::NativePreConstruct()
{
	Super::NativePreConstruct();
}
