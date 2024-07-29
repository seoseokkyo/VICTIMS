// Fill out your copyright notice in the Description page of Project Settings.


#include "Victims_ChatMsg.h"

#include <string>
#include "ChatChannelTypeDef.h"
#include "Components/TextBlock.h"

void UVictims_ChatMsg::NativeConstruct()
{
	Super::NativeConstruct();

	if (MsgInfo.Channel != EChatChannelType::None)
	{
		// Set msg color
		Msg->SetColorAndOpacity(MsgInfo.MsgColor);

		const UEnum* CurrentEnum{ FindFirstObjectSafe<UEnum>(TEXT("EChatChannelType")) };

		// Set msg text

		const FString IsSenderPrefix{ MsgInfo.bIsSender ? FString("To. ") : FString("From. ") },
			IsSenderSuffix{ MsgInfo.bIsSender ? MsgInfo.Recipient + FString(" <<<") : MsgInfo.SenderName + FString(" >>>") },
			EmptyString{ FString("") };

		Msg->SetText(FText::FromString(
			/*TimeStamp*/(MsgInfo.bShowTimeStamp ? std::to_string(MsgInfo.MsgTime.GetHour12()).c_str() +
				FString(":") + std::to_string(MsgInfo.MsgTime.GetMinute()).c_str() + FString(" ") : EmptyString) +
			/*Channel*/(MsgInfo.Channel != EChatChannelType::Private ? FString("[ ") +
				CurrentEnum->GetNameStringByValue((int64)MsgInfo.Channel) + FString(" ] ") : IsSenderPrefix + IsSenderSuffix) +
			/*Name*/(MsgInfo.SenderName.IsEmpty() ? FString(": ") : (MsgInfo.Channel != EChatChannelType::Private ? (MsgInfo.bIsSender ? MsgInfo.Recipient :
				MsgInfo.SenderName) : EmptyString) + FString(" : ")) + MsgInfo.Msg));
		//CurrentEnumÀÌ null
		// Set msg font
		Msg->SetFont(MsgInfo.MsgFontInfo);
	}
}