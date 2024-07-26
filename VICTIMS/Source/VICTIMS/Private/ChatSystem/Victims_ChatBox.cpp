// Fill out your copyright notice in the Description page of Project Settings.


#include "Victims_ChatBox.h"

#include "VictimsChatManager.h"
#include <Components/Border.h>
#include <Components/Button.h>
#include <Components/EditableTextBox.h>
#include <Components/HorizontalBox.h>
#include <Components/HorizontalBoxSlot.h>
#include <Components/Image.h>
#include <Components/ScrollBox.h>
#include <Components/TextBlock.h>
#include <Components/VerticalBox.h>
#include "Victims_ChatMsg.h"
#include "Victims_ContainerTab.h"
#include <Framework/Application/SlateApplication.h>
#include <Engine/GameInstance.h>
#include <GameFramework/PlayerController.h>
#include "Victims_ChatSubsystem.h"

void UVictims_ChatBox::UpdateCurrentChatBoxMessages(UVictims_ContainerTab* InTarget) const
{
	if (!InTarget)
	{
		return;
	}

	VB_Msg->ClearChildren();
	for (UVictims_ChatMsg* Elem : InTarget->Msges)
	{
		VB_Msg->AddChildToVerticalBox(Elem);
		// UE_LOG(LogTemp, Error, L"%s.", *Elem->MsgInfo.Msg);
	}
	ScrollBox->ScrollToStart();
}

void UVictims_ChatBox::UpdateMsgInChatBox(UUserWidget* InNewChat) const
{
	VB_Msg->AddChild(InNewChat);

	if (const float CurrentScrollOffset{ ScrollBox->GetScrollOffset() }; CurrentScrollOffset <= 0.f)
	{
		ScrollBox->ScrollToStart();
	}
	else
	{
		ScrollBox->SetScrollOffset(CurrentScrollOffset + VB_Msg->GetChildAt(0)->GetDesiredSize().Y);
	}
}

void UVictims_ChatBox::NativeConstruct()
{
	Super::NativeConstruct();

	// Since the "GetOwningPlayer()" function is a local function, there is no need to check it with functions
	// such as "IsLocallyControl"
	if (APlayerController * CurrentOwner{ GetOwningPlayer() })
	{
		// Bind
		MsgTextBox->OnTextChanged.AddUniqueDynamic(this, &ThisClass::OnMsgTextChangedFunc);
		// FoldBtn->OnClicked.AddUniqueDynamic(this, &ThisClass::GmOnClickedFoldBtn);

		const UVictimsChatManager* ChatManager{ CurrentOwner->FindComponentByClass<UVictimsChatManager>() };
		// UGmAC_ChatManager* ChatManager{ChatSubsystem ? ChatSubsystem->GetChatManagerByOwningActor(CurrentOwner) : nullptr};
		if (!ChatManager)
		{
			UE_LOG(LogTemp, Error, L"ChatBox->NativeConstruct() - ChatManager is not valid.");
			return;
		}

		CmdPrefix = ChatManager->CmdPrefix;
		ArrM_CmdChannels = ChatManager->ArrM_CmdChannels;
		ArrM_GeneralInfoByChannel = ChatManager->ArrM_GeneralInfoByChannel;

		if (ContainerTabClassRef)
		{
			if (UVictims_ContainerTab * DefaultTab{ CreateWidget<UVictims_ContainerTab>(CurrentOwner, ContainerTabClassRef) })
			{
				DefaultTab->MainChatBoxRef = this;
				DefaultTab->SetCurrentChatTabOpened();

				if (UHorizontalBoxSlot * HB_Slot{ static_cast<UHorizontalBoxSlot*>(HB_Tabs->AddChild(DefaultTab)) })
				{
					FSlateChildSize NewSize;
					NewSize.SizeRule = ESlateSizeRule::Automatic;
					HB_Slot->SetSize(NewSize);
					HB_Slot->SetPadding(FMargin(0));
				}
				// Initial configuration for channels where chat is visible
				for (auto temp : ChatManager->ArrM_FilteringSettingsForEachChannel)
				{
					DefaultTab->ArrM_FilteringSettingsForEachChannel.Add(static_cast<uint8>(temp.Key), temp.Value);
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, L"ChatBox - ContainerTabClass Ref is not valid.");
		}
	}
}

/*FReply UVictims_ChatBox::NativeOnPreviewKeyDown(const FGeometry & InGeometry, const FKeyEvent & InKeyEvent)
	{
		if (const FKey PressedKey{ InKeyEvent.GetKey() }; PressedKey == EKeys::Enter)
		{
			if (MsgTextBox->HasKeyboardFocus())
			{
				SendMsg(MsgTextBox->GetText().ToString());
				ToggleChatBox(false);
			}
			else if (!PrivateRecipient.IsEmpty())
			{
				UpdateCurrentChannel(PrivateRecipient.IsEmpty() ? MsgHistory[HistoryCounter].Channel : EChatChannelType::Private,
					PrivateRecipient.IsEmpty() ? MsgHistory[HistoryCounter].Recipient : PrivateRecipient);
				UpdateTextColor();
				UpdateTypingInChannel();
				FocusChat(false, false);
			}
			return FReply::Handled();
		}
		else if (PressedKey == EKeys::Up && MsgHistory.IsValidIndex(HistoryCounter - 1))
		{
			--HistoryCounter;
		}
		else if (PressedKey == EKeys::Down && MsgHistory.IsValidIndex(HistoryCounter + 1))
		{
			++HistoryCounter;
		}
		else if (PressedKey == EKeys::Tab && MsgTextBox->HasKeyboardFocus())
		{
			PrivateRecipient = L"";
			int32 NewChannelIndex{ (int32)CurrentChannel };
			do
			{
				++NewChannelIndex;

				if (NewChannelIndex > (int32)EChatChannelType::SystemLog)
				{
					NewChannelIndex = 1;
				}
				CurrentChannel = static_cast<EChatChannelType>(NewChannelIndex);
			} while (CurrentChannel == EChatChannelType::None || CurrentChannel == EChatChannelType::Private ||
				CurrentChannel == EChatChannelType::Announcement || CurrentChannel == EChatChannelType::Game ||
				CurrentChannel == EChatChannelType::SystemLog);

			UpdateTypingInChannel();
			UpdateCurrentChannel(CurrentChannel, PrivateRecipient);
			UpdateTextColor();
			return FReply::Handled();
		}
		else
		{
			return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
		}

		UpdateCurrentChannel(PrivateRecipient.IsEmpty() ? MsgHistory[HistoryCounter].Channel : EChatChannelType::Private,
			PrivateRecipient.IsEmpty() ? MsgHistory[HistoryCounter].Recipient : PrivateRecipient);
		UpdateTextColor();
		UpdateTypingInChannel();
		FocusChat(false, false);
		MsgTextBox->SetText(FText::FromString(MsgHistory[HistoryCounter].Msg));

		return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
	}

	*/



void UVictims_ChatBox::FocusChat(const bool InIsSlash, const bool InIsRemove)
{
	if (InIsRemove)
	{
		MsgTextBox->SetText(FText::GetEmpty());
		CurrentTypingChannelText->SetText(FText::GetEmpty());
		FSlateApplication::Get().SetAllUserFocusToGameViewport(); /* SetFocusToViewport() */
		return;
	}

	MsgTextBox->SetKeyboardFocus();

	if (InIsSlash)
	{
		MsgTextBox->SetText(FText::FromString("/"));
	}
	else
	{
		UpdateTypingInChannel();
	}

	UpdateTextColor();
	ToggleChatBox(true);
}

void UVictims_ChatBox::UpdateTypingInChannel()
{
	FText NewText;

	if (CurrentChannel != EChatChannelType::Private)
	{
		NewText = FText::FromString(FString("[") + GetEnumDisplayString(L"EChatChannelType",
			(int64)CurrentChannel) + FString("] : "));
	}
	else
	{
		NewText = FText::FromString(FString("To. ") + PrivateRecipient + FString(" <<< : "));
	}
	CurrentTypingChannelText->SetText(NewText);
}

void UVictims_ChatBox::UpdateCurrentChannel(const EChatChannelType InNewChannel, const FString& InPrivateRecipient)
{
	CurrentChannel = InNewChannel;
	PrivateRecipient = InPrivateRecipient;
}

void UVictims_ChatBox::UpdateTextColor()
{
	TextColor = ArrM_GeneralInfoByChannel.Contains(CurrentChannel) ?
		ArrM_GeneralInfoByChannel[CurrentChannel].TextColor : FLinearColor::White;

	UE_CLOG(!ArrM_GeneralInfoByChannel.Contains(CurrentChannel), LogTemp, Error,
		L"Main Chat Box Widget - Color by channel is not valid.");

	MsgTextBox->WidgetStyle.SetForegroundColor(TextColor);
	CurrentTypingChannelText->SetColorAndOpacity(TextColor);
}



FReply UVictims_ChatBox::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{

	if (const FKey PressedKey{ InKeyEvent.GetKey() }; PressedKey == EKeys::Enter)
	{
		if (MsgTextBox->HasKeyboardFocus())
		{
			SendMsg(MsgTextBox->GetText().ToString());
			ToggleChatBox(false);
		}
		else if (!PrivateRecipient.IsEmpty())
		{
			UpdateCurrentChannel(PrivateRecipient.IsEmpty() ? MsgHistory[HistoryCounter].Channel : EChatChannelType::Private,
				PrivateRecipient.IsEmpty() ? MsgHistory[HistoryCounter].Recipient : PrivateRecipient);
			UpdateTextColor();
			UpdateTypingInChannel();
			FocusChat(false, false);
		}
		return FReply::Handled();
	}
	else if (PressedKey == EKeys::Up && MsgHistory.IsValidIndex(HistoryCounter - 1))
	{
		--HistoryCounter;
	}
	else if (PressedKey == EKeys::Down && MsgHistory.IsValidIndex(HistoryCounter + 1))
	{
		++HistoryCounter;
	}
	else if (PressedKey == EKeys::Tab && MsgTextBox->HasKeyboardFocus())
	{
		PrivateRecipient = L"";
		int32 NewChannelIndex{ (int32)CurrentChannel };
		do
		{
			++NewChannelIndex;

			if (NewChannelIndex > (int32)EChatChannelType::SystemLog)
			{
				NewChannelIndex = 1;
			}
			CurrentChannel = static_cast<EChatChannelType>(NewChannelIndex);
		} while (CurrentChannel == EChatChannelType::None || CurrentChannel == EChatChannelType::Private ||
			CurrentChannel == EChatChannelType::Announcement || CurrentChannel == EChatChannelType::Game ||
			CurrentChannel == EChatChannelType::SystemLog);

		UpdateTypingInChannel();
		UpdateCurrentChannel(CurrentChannel, PrivateRecipient);
		UpdateTextColor();
		return FReply::Handled();
	}
	else
	{
		return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
	}

	UpdateCurrentChannel(PrivateRecipient.IsEmpty() ? MsgHistory[HistoryCounter].Channel : EChatChannelType::Private,
		PrivateRecipient.IsEmpty() ? MsgHistory[HistoryCounter].Recipient : PrivateRecipient);
	UpdateTextColor();
	UpdateTypingInChannel();
	FocusChat(false, false);
	MsgTextBox->SetText(FText::FromString(MsgHistory[HistoryCounter].Msg));

	return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
}



void UVictims_ChatBox::ToggleChatBox(const bool InIsShow)
{
	if (InIsShow)
	{
		MsgTextBox->WidgetStyle.SetForegroundColor(ArrM_GeneralInfoByChannel.Contains(CurrentChannel) ?
			ArrM_GeneralInfoByChannel[CurrentChannel].TextColor : FLinearColor(1, 1, 1, 1));

		MsgTextBox->WidgetStyle.SetBackgroundColor(FLinearColor(0, 0, 0, 0));

		ChatBorder->SetBrushColor(BackgroundBorderColor);
		BGBorder->SetBrushColor(BackgroundBorderColor);
		BGImage->SetColorAndOpacity(BackgroundBorderColor);

		for (UWidget* Elem : HB_Tabs->GetAllChildren())
		{
			if (UVictims_ContainerTab * ContainerTab{ static_cast<UVictims_ContainerTab*>(Elem) })
			{
				ContainerTab->TabName->SetVisibility(ESlateVisibility::Visible);
				ContainerTab->TabBtn->SetBackgroundColor(ContainerTab == CurrentOpenedTab ?
					FLinearColor(1.f, 1.f, 1.f, 0.75f) : FLinearColor(1, 1, 1, 1));
			}
		}

		AddTabBtn->SetVisibility(ESlateVisibility::Visible);
		ScrollBox->SetVisibility(ESlateVisibility::Visible);
	}
	else if (bAutoHide)
	{
		MsgTextBox->WidgetStyle.SetBackgroundColor(FLinearColor(0, 0, 0, 0));

		ChatBorder->SetBrushColor(FLinearColor(0, 0, 0, 0));
		BGBorder->SetBrushColor(FLinearColor(0, 0, 0, 0));
		BGImage->SetColorAndOpacity(FLinearColor(0, 0, 0, 0));

		for (UWidget* Elem : HB_Tabs->GetAllChildren())
		{
			if (const UVictims_ContainerTab * ContainerTab{ static_cast<UVictims_ContainerTab*>(Elem) })
			{
				ContainerTab->TabName->SetVisibility(ESlateVisibility::Collapsed);
				ContainerTab->TabBtn->SetBackgroundColor(FLinearColor(0, 0, 0, 0));
			}
		}

		AddTabBtn->SetVisibility(ESlateVisibility::Collapsed);
		ScrollBox->SetVisibility(ESlateVisibility::Collapsed);
	}
}

//@TODO Move the implementation of this item to the replicated ChatManager class.
void UVictims_ChatBox::SendMsg(const FString& InMsg)
{
	UVictims_ChatSubsystem* ChatSubsystem{ GetGameInstance()->GetSubsystem<UVictims_ChatSubsystem>() };
	APlayerController* OwningPC{ GetOwningPlayer() };

	if (!ChatSubsystem || !OwningPC)
	{
		UE_LOG(LogTemp, Error, L"%hs() - ChatSubsystem or OwningPC is not valid.", __FUNCTION__);
		return;
	}

	if (InMsg.IsEmpty())
	{
		FocusChat(false, true);
		return;
	}

	const FString WhiteSpace{ FString(" ") };
#if WITH_EDITOR
	UE_CLOG(ChatSubsystem->GetIsDebuggingModeFromOwningActor(OwningPC), LogTemp, Error, L"SendMsg() : %s.", *InMsg);
#endif

	// Start with command prefix
	if (InMsg.StartsWith(CmdPrefix))
	{
		FString CharExCmd, RightS;
		if (InMsg.Contains(WhiteSpace))
		{
			FString LeftS;
			InMsg.Split(WhiteSpace, &LeftS, &RightS);
			CharExCmd = LeftS.Right(LeftS.Len() - CmdPrefix.Len());
		}
		else
		{
			CharExCmd = InMsg.Right(InMsg.Len() - CmdPrefix.Len());
		}

		// Valid cmd prefix
		if (!CharExCmd.IsEmpty() && ArrM_CmdChannels.Contains(CharExCmd))
		{
			MsgTextBox->SetText(FText::GetEmpty());

			// Create party
			if (CharExCmd.Equals(FString("CreateParty"), ESearchCase::IgnoreCase))
			{
#if WITH_EDITOR
				UE_CLOG(ChatSubsystem->GetIsDebuggingModeFromOwningActor(OwningPC), LogTemp, Error, L"CreateParty");
#endif
				ChatSubsystem->CreateNewParty(OwningPC);
			}

			// Invite new party member
			//@TODO Be sure to refactor the command logic later.
			if (CharExCmd.Equals(FString("Invite"), ESearchCase::IgnoreCase))
			{
#if WITH_EDITOR
				UE_CLOG(ChatSubsystem->GetIsDebuggingModeFromOwningActor(OwningPC), LogTemp, Error, L"Invite");
#endif
				ChatSubsystem->SendPartyInvitationsByName(OwningPC, RightS.TrimStartAndEnd());
			}

			// Break party
			//@TODO Be sure to refactor the command logic later.
			if (CharExCmd.Equals(FString("BreakParty"), ESearchCase::IgnoreCase))
			{
#if WITH_EDITOR
				UE_CLOG(ChatSubsystem->GetIsDebuggingModeFromOwningActor(OwningPC), LogTemp, Error, L"BreakParty");
#endif
				ChatSubsystem->BreakParty(OwningPC);
			}

			// Leave party
			//@TODO Be sure to refactor the command logic later.
			if (CharExCmd.Equals(FString("LeaveParty"), ESearchCase::IgnoreCase))
			{
#if WITH_EDITOR
				UE_CLOG(ChatSubsystem->GetIsDebuggingModeFromOwningActor(OwningPC), LogTemp, Error, L"LeaveParty");
#endif
				ChatSubsystem->LeaveParty(OwningPC);
			}

			// Send whisper message
			//@TODO Be sure to refactor the command logic later.
			if (CharExCmd.Equals(FString("w"), ESearchCase::IgnoreCase) && !RightS.IsEmpty())
			{
				FString RightL, RightR;
				RightS.Split(WhiteSpace, &RightL, &RightR, ESearchCase::IgnoreCase);
				if (!RightR.IsEmpty())
				{
#if WITH_EDITOR
					UE_CLOG(true, LogTemp, Error, L"Whisper() CharExCmd : %s, RightL : %s, RightR: %s.",
						*CharExCmd, *RightL, *RightR);
#endif

					const EChatChannelType CurrentTargetChannel{ ArrM_CmdChannels.Contains(CharExCmd) ?
						ArrM_CmdChannels[CharExCmd].ChannelToTransmitExplanation : EChatChannelType::Private };

					PrivateRecipient = RightL;
					UpdateTypingInChannel();

					ChatSubsystem->SendWhisperMsg(OwningPC, RightL,
						FVictims_MsgInfo(FDateTime::Now(), RightR, ChatSubsystem->GetPlayerNameByOwningActor(OwningPC),
							CurrentTargetChannel, ArrM_GeneralInfoByChannel.Contains(CurrentTargetChannel) ?
							ArrM_GeneralInfoByChannel[CurrentTargetChannel].TextColor : FSlateColor(),
							FString(""), true, true,
							ArrM_CmdChannels[CharExCmd].CmdFontInfo));
				}
			}

			FocusChat(false, true);
			return;
		}//~End of Start with cmd prefix + cmd content
	}

	// Message, not command

	const FVictims_MsgInfo NewMsgInfo{ FVictims_MsgInfo(/*@TODO Fix it to get server time from server*/FDateTime::Now(),
	TrimPrecedingAndTrailing(InMsg).ToString(), ChatSubsystem->GetPlayerNameByOwningActor(OwningPC), CurrentChannel, TextColor,
	PrivateRecipient, /*Not Sender*/false, bShowTimeStamps) };

	ChatSubsystem->ReceiveMsg(OwningPC, NewMsgInfo);

	MsgTextBox->SetText(FText::GetEmpty());
	// FocusChat(false, true);
	SaveMsg(NewMsgInfo);

	FocusChat(false, true);
}

void UVictims_ChatBox::SaveMsg(const FVictims_MsgInfo& InMsg)
{
	HistoryCounter = MsgHistory.Add(InMsg) + 1;
}

FText UVictims_ChatBox::TrimPrecedingAndTrailing(const FString& InText)
{
	int32 StartPos{ 0 };
	while (StartPos < InText.Len())
	{
		if (!FText::IsWhitespace(InText[StartPos]))
		{
			break;
		}

		++StartPos;
	}

	int32 EndPos{ InText.Len() - 1 };
	while (EndPos > StartPos)
	{
		if (!FText::IsWhitespace(InText[EndPos]))
		{
			break;
		}

		--EndPos;
	}

	if (StartPos == 0 && EndPos == InText.Len() - 1)
	{
		// Nothing to trim!
		return FText::FromString(InText);
	}

	// Trim the string, preserving culture invariance if set
	FString TrimmedString{ InText.Mid(StartPos, EndPos - StartPos + 1) };

	return FText::FromString(InText).IsCultureInvariant() ?
		FText::AsCultureInvariant(MoveTemp(TrimmedString)) : FText::FromString(MoveTemp(TrimmedString));
}

void UVictims_ChatBox::OnMsgTextChangedFunc(const FText& InText)
{
	UpdateTypingInChannel();

	// Start with command prefix
	if (const FString TargetMsg{ InText.ToString() }; TargetMsg.StartsWith(CmdPrefix))
	{
		FString CharExCmd, RightS;
		if (TargetMsg.Contains(FString(" ")))
		{
			FString LeftS;
			TargetMsg.Split(FString(" "), &LeftS, &RightS);
			CharExCmd = LeftS.Right(LeftS.Len() - CmdPrefix.Len());
		}
		else
		{
			CharExCmd = TargetMsg.Right(TargetMsg.Len() - CmdPrefix.Len());
		}

		if (!CharExCmd.IsEmpty() && ArrM_CmdChannels.Contains(CharExCmd))
		{
			if (ArrM_CmdChannels[CharExCmd].ChannelToTransmitExplanation == EChatChannelType::None)
			{
				return;
			}
			if (CharExCmd == GetEnumDisplayString(L"EChatChannelType", (int64)CurrentChannel))
			{
				MsgTextBox->SetText(FText::GetEmpty());
				UpdateCurrentChannel(ArrM_CmdChannels[CharExCmd].ChannelToTransmitExplanation,
					ArrM_CmdChannels[CharExCmd].ChannelToTransmitExplanation == EChatChannelType::Private ?
					RightS : FString(""));
				UpdateTextColor();
				UpdateTypingInChannel();
			}
		}
	}
}



FString UVictims_ChatBox::GetEnumDisplayString(const TCHAR* InEnumName, const int64 InEnumValue)
{
	return FindFirstObjectSafe<UEnum>(InEnumName)->GetNameStringByValue(InEnumValue);
}


