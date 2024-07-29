// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatChannelTypeDef.h"
#include "Victims_ChatBox.generated.h"

class UVictims_ContainerTab;
class UEditableTextBox;
class UTextBlock;
class UBorder;
class UImage;
class UScrollBox;
class UHorizontalBox;
class UVerticalBox;
class UButton;

//UENUM(BlueprintType)
//enum class EChatChannelType : uint8
//{
//	None = 0 UMETA(DisplayName = None),
//	Global = 1 UMETA(DisplayName = Global),
//	Friend = 2 UMETA(DisplayName = Friend),
//	Party = 3 UMETA(DisplayName = Party),
//	Private = 4 UMETA(DisplayName = Private),
//	Announcement = 5 UMETA(DisplayName = Announcement),
//	Game = 6 UMETA(DisplayName = Game),
//	SystemLog = 7 UMETA(DisplayName = SystemLog)
//};

USTRUCT(BlueprintType)
struct FVictims_MsgInfo
{
	GENERATED_BODY()



	FVictims_MsgInfo()
		:
		MsgTime(), Channel(), bIsSender(false)
	{}

	FVictims_MsgInfo(const FDateTime InMsgTime, const FString& InMsg, const EChatChannelType InChannel,
		const bool InShowTimeStamp)
		:
		MsgTime(InMsgTime),
		Msg(InMsg),
		Channel(InChannel),
		bShowTimeStamp(InShowTimeStamp)
	{}

	FVictims_MsgInfo(const FDateTime InMsgTime, const FString& InMsg, const FString& InSenderName,
		const EChatChannelType InChannel, const FSlateColor& InMsgColor, const FString& InRecipient,
		const bool InIsSender, const bool InShowTimeStamp, const FSlateFontInfo& InFontInfo = FSlateFontInfo())
		:
		MsgTime(InMsgTime),
		Msg(InMsg),
		SenderName(InSenderName),
		Channel(InChannel),
		MsgColor(InMsgColor),
		Recipient(InRecipient),
		bIsSender(InIsSender),
		bShowTimeStamp(InShowTimeStamp)
	{}

	FDateTime MsgTime;

	UPROPERTY()
	FString Msg;
	UPROPERTY()
	FString SenderName;
	UPROPERTY()
	EChatChannelType Channel;
	UPROPERTY()
	FSlateColor MsgColor;
	UPROPERTY()
	FString Recipient;
	UPROPERTY()
	bool bIsSender{ false };
	UPROPERTY()
	bool bShowTimeStamp{ true };
	UPROPERTY()
	FSlateFontInfo MsgFontInfo;
};

USTRUCT(BlueprintType)
struct FChatCmdDescOfChannel
{
	GENERATED_BODY()

	FChatCmdDescOfChannel()
		:
		DescOfChannel(FString("")),
		ChannelToTransmitExplanation(EChatChannelType::None)
	{}

	FChatCmdDescOfChannel(const FString& InDescriptionOfChannel, const EChatChannelType InChannelToTransmitExplanation)
		:
		DescOfChannel(InDescriptionOfChannel),
		ChannelToTransmitExplanation(InChannelToTransmitExplanation)
	{}

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Init Settings")
	FString DescOfChannel;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Init Settings")
	EChatChannelType ChannelToTransmitExplanation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Init Settings", DisplayName = "Command Notify Text Font Info")
	FSlateFontInfo CmdFontInfo;
};

USTRUCT(BlueprintType)
struct FChatGeneralInfo
{
	GENERATED_BODY()

	FChatGeneralInfo()
		:
		TextColor(FLinearColor::White),
		TextFontInfo()
	{}

	explicit FChatGeneralInfo(const FLinearColor& InTextColor,
		const FSlateFontInfo& InTextFontInfo = FSlateFontInfo())
		:
		TextColor(InTextColor),
		TextFontInfo(InTextFontInfo)
	{}

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Init Settings")
	FLinearColor TextColor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Init Settings")
	FSlateFontInfo TextFontInfo;

};


UCLASS()
class VICTIMS_API UVictims_ChatBox : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> MsgTextBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CurrentTypingChannelText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> ChatBorder;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> BGBorder;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> BGImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> ScrollBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> HB_Tabs;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> VB_Msg;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<UButton> AddTabBtn;

	EChatChannelType CurrentChannel{ EChatChannelType::Global };

	TSubclassOf<UVictims_ContainerTab> ContainerTabClassRef;

	UPROPERTY()
	FString PrivateRecipient;

	UPROPERTY(EditDefaultsOnly, Category = "Init Settings")
	FLinearColor BackgroundBorderColor{ FLinearColor(0.019608f, 0.019608f, 0.019608f, 0.5f) };

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	bool bShowTimeStamps{ true };

	UPROPERTY(EditAnywhere, Category = Settings)
	bool bAutoHide;

	UPROPERTY()
	FSlateColor TextColor;

	UPROPERTY()
	TObjectPtr<UVictims_ContainerTab> CurrentOpenedTab;

	// Utils
	UFUNCTION(BlueprintCallable)
	void UpdateCurrentChatBoxMessages(UVictims_ContainerTab* InTarget) const;
	UFUNCTION(BlueprintCallable)
	void UpdateMsgInChatBox(UUserWidget* InNewChat) const;
	UFUNCTION(BlueprintCallable)
	void ToggleChatBox(bool InIsShow);
	UFUNCTION(BlueprintCallable)
	void UpdateCurrentChannel(EChatChannelType InNewChannel, const FString& InPrivateRecipient);
	UFUNCTION(BlueprintCallable)
	void FocusChat(bool InIsSlash, bool InIsRemove);
	UFUNCTION(BlueprintCallable)
	void UpdateTypingInChannel();
	UFUNCTION(BlueprintCallable)
	void UpdateTextColor();



protected:

	// UUserWidget Interface
	virtual void NativeConstruct() override;
	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	//~End of UUserWidget Interface

	

private:

	friend class UGmAC_ChatManager;

	UFUNCTION(BlueprintCallable)
	void SendMsg(const FString& InMsg);

	UFUNCTION()
	void SaveMsg(const FVictims_MsgInfo& InMsg);

	FString CmdPrefix{ FString("/") };
	TArray<FVictims_MsgInfo> MsgHistory;

	int32 HistoryCounter;

	TMap<EChatChannelType, FChatGeneralInfo> ArrM_GeneralInfoByChannel;
	TMap<FString, FChatCmdDescOfChannel> ArrM_CmdChannels;

	UFUNCTION(BlueprintCallable)
	void OnMsgTextChangedFunc(const FText& InText);

	static FText TrimPrecedingAndTrailing(const FString& InText);
	static FString GetEnumDisplayString(const TCHAR* InEnumName, const int64 InEnumValue);
};
