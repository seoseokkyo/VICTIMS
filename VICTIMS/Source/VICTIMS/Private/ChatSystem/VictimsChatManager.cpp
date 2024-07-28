// Fill out your copyright notice in the Description page of Project Settings.


#include "VictimsChatManager.h"

#include <string>

#include <EngineUtils.h>
#include <Blueprint/UserWidget.h>
#include <Components/HorizontalBox.h>
#include <Net/UnrealNetwork.h>

#include "Victims_ChatBox.h"
#include "Victims_ChatMsg.h"
#include "Victims_ContainerTab.h"

#include <UObject/ConstructorHelpers.h>
#include <Engine/World.h>
#include <TimerManager.h>
#include <Components/EditableText.h>
#include <Components/EditableTextBox.h>
#include <GameFramework/PlayerState.h>
#include <Components/InputComponent.h>
#include <GameFramework/Pawn.h>
#include <Engine/GameInstance.h>
#include "Victims_ChatSubsystem.h"
#include "Victims_FloatingWidget.h"
#include <AVICTIMSPlayerController.h>


UVictimsChatManager::UVictimsChatManager(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	SetIsReplicatedByDefault(true);
	bReplicateUsingRegisteredSubObjectList = true;

	struct FHiHoRefAssist
	{
		ConstructorHelpers::FClassFinder<UVictims_ChatBox> MainChatWidgetClassPath;
		ConstructorHelpers::FClassFinder<UVictims_ChatMsg> MsgWidgetClassPath;
		ConstructorHelpers::FClassFinder<UVictims_ContainerTab> ContainerTabWidgetClassPath;
		ConstructorHelpers::FObjectFinder<UObject> DefaultFont;
		ConstructorHelpers::FClassFinder<UVictims_FloatingWidget> WhisperServerWidgetPath;

		FHiHoRefAssist()
			:
			MainChatWidgetClassPath(L"/Script/UMGEditor.WidgetBlueprint'/GmDynamicMultiChatSystem/Blueprints/Widgets/BP_ChatBox'"),
			MsgWidgetClassPath(L"/Script/UMGEditor.WidgetBlueprint'/GmDynamicMultiChatSystem/Blueprints/Widgets/BP_ChatMessage'"),
			ContainerTabWidgetClassPath(L"/Script/UMGEditor.WidgetBlueprint'/GmDynamicMultiChatSystem/Blueprints/Widgets/BP_ContainerTab'"),
			DefaultFont(L"/Script/Engine.Font'/Engine/EngineFonts/Roboto'"),
			WhisperServerWidgetPath(L"/Script/UMGEditor.WidgetBlueprint'/GmDynamicMultiChatSystem/Blueprints/Widgets/Notify/BP_WhisperServer'")
		{}
	};
	///Game/MyCustomFolder/Blueprints/Widgets/BP_ChatBox
	static FHiHoRefAssist Ho;

	if (!ChatWidgetClassRef && Ho.MainChatWidgetClassPath.Succeeded())
	{
		ChatWidgetClassRef = Ho.MainChatWidgetClassPath.Class;
	}
	if (!MsgWidgetClassRef && Ho.MsgWidgetClassPath.Succeeded())
	{
		MsgWidgetClassRef = Ho.MsgWidgetClassPath.Class;
	}
	if (!ContainerTabClassRef && Ho.ContainerTabWidgetClassPath.Succeeded())
	{
		ContainerTabClassRef = Ho.ContainerTabWidgetClassPath.Class;
	}
	if (!WhisperWidgetClassRef && Ho.WhisperServerWidgetPath.Succeeded())
	{
		WhisperWidgetClassRef = Ho.WhisperServerWidgetPath.Class;
	}

	if (!ArrM_CmdChannels.Contains(FString("CreateParty")))
	{
		ArrM_CmdChannels.Add(FString("CreateParty")) = FChatCmdDescOfChannel(FString("A new party has been created."), EChatChannelType::Game);
	}
	if (!ArrM_CmdChannels.Contains(FString("Invite")))
	{
		ArrM_CmdChannels.Add(FString("Invite")) = FChatCmdDescOfChannel(FString("Party invitation."), EChatChannelType::Party);
	}
	if (!ArrM_CmdChannels.Contains(FString("BreakParty")))
	{
		ArrM_CmdChannels.Add(FString("BreakParty")) = FChatCmdDescOfChannel(FString("The party was broken up."), EChatChannelType::Game);
	}
	if (!ArrM_CmdChannels.Contains(FString("LeaveParty")))
	{
		ArrM_CmdChannels.Add(FString("LeaveParty")) = FChatCmdDescOfChannel(FString("left the party."), EChatChannelType::Game);
	}
	if (!ArrM_CmdChannels.Contains(FString("Help")))
	{
		ArrM_CmdChannels.Add(FString("Help")) = FChatCmdDescOfChannel(FString("You want a help? XD"), EChatChannelType::Game);
	}
	if (!ArrM_CmdChannels.Contains(FString("w")))
	{
		ArrM_CmdChannels.Add(FString("w")) = FChatCmdDescOfChannel(FString(""), EChatChannelType::Private);
	}//@TODO Just before packaging for shipping, please set all initial settings for this section and comment or delete this section.

	const FSlateFontInfo NewDefaultFontInfo(FSlateFontInfo(Ho.DefaultFont.Object, 13)),
		NewSystemFontInfo(FSlateFontInfo(Ho.DefaultFont.Object, 11));

	if (!ArrM_GeneralInfoByChannel.Contains(EChatChannelType::Global))
	{
		ArrM_GeneralInfoByChannel.Add(EChatChannelType::Global) = FChatGeneralInfo(FLinearColor(0.85f, 0.85f, 0.85f, 0.85f), NewDefaultFontInfo);
	}
//	if (!ArrM_GeneralInfoByChannel.Contains(EChatChannelType::Friend))
//	{
//		ArrM_GeneralInfoByChannel.Add(EChatChannelType::Friend) = FChatGeneralInfo(FLinearColor(1.f, 0.65f, 0.f, 1.f), NewDefaultFontInfo);/*Orange*/
//	}
	if (!ArrM_GeneralInfoByChannel.Contains(EChatChannelType::Party))
	{
		ArrM_GeneralInfoByChannel.Add(EChatChannelType::Party) = FChatGeneralInfo(FLinearColor(0.3137f, 0.73725f, 0.913725f, 1.f), NewDefaultFontInfo);/*Sky blue*/
	}
	if (!ArrM_GeneralInfoByChannel.Contains(EChatChannelType::Private))
	{
		ArrM_GeneralInfoByChannel.Add(EChatChannelType::Private) = FChatGeneralInfo(FLinearColor(0.f, 1.f, 0.f, 1.f), NewDefaultFontInfo);/*Green*/
	}
	if (!ArrM_GeneralInfoByChannel.Contains(EChatChannelType::Announcement))
	{
		ArrM_GeneralInfoByChannel.Add(EChatChannelType::Announcement) = FChatGeneralInfo(FLinearColor::Yellow, NewDefaultFontInfo);
	}
	if (!ArrM_GeneralInfoByChannel.Contains(EChatChannelType::Game))
	{
		ArrM_GeneralInfoByChannel.Add(EChatChannelType::Game) = FChatGeneralInfo(FLinearColor(0.8f, 0.8f, 0.8f, 0.5f), NewSystemFontInfo);
	}
	if (!ArrM_GeneralInfoByChannel.Contains(EChatChannelType::SystemLog))
	{
		ArrM_GeneralInfoByChannel.Add(EChatChannelType::SystemLog) = FChatGeneralInfo(FLinearColor(0.9f, 0.9f, 0.9f, 0.9f), NewSystemFontInfo);
	}
	/////////////////////////////////////////////////////////////////////////////////
	if (!ArrM_FilteringSettingsForEachChannel.Contains(EChatChannelType::None))
	{
		ArrM_FilteringSettingsForEachChannel.Add(EChatChannelType::None) = false;
	}
	if (!ArrM_FilteringSettingsForEachChannel.Contains(EChatChannelType::Global))
	{
		ArrM_FilteringSettingsForEachChannel.Add(EChatChannelType::Global) = true;
	}
//	if (!ArrM_FilteringSettingsForEachChannel.Contains(EChatChannelType::Friend))
//	{
//		ArrM_FilteringSettingsForEachChannel.Add(EChatChannelType::Friend) = true;
//	}
	if (!ArrM_FilteringSettingsForEachChannel.Contains(EChatChannelType::Party))
	{
		ArrM_FilteringSettingsForEachChannel.Add(EChatChannelType::Party) = true;
	}
	if (!ArrM_FilteringSettingsForEachChannel.Contains(EChatChannelType::Private))
	{
		ArrM_FilteringSettingsForEachChannel.Add(EChatChannelType::Private) = true;
	}
	if (!ArrM_FilteringSettingsForEachChannel.Contains(EChatChannelType::Announcement))
	{
		ArrM_FilteringSettingsForEachChannel.Add(EChatChannelType::Announcement) = true;
	}
	if (!ArrM_FilteringSettingsForEachChannel.Contains(EChatChannelType::Game))
	{
		ArrM_FilteringSettingsForEachChannel.Add(EChatChannelType::Game) = true;
	}
	if (!ArrM_FilteringSettingsForEachChannel.Contains(EChatChannelType::SystemLog))
	{
		ArrM_FilteringSettingsForEachChannel.Add(EChatChannelType::SystemLog) = true;
	}//@~TODO Just before packaging for shipping, please set all initial settings for this section and comment or delete this section.

	PlayerNameDisallowedChars.AddUnique(FString(" "));
	PlayerNameDisallowedChars.AddUnique(FString("\n"));
}

void UVictimsChatManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, PlayerName);
	DOREPLIFETIME(ThisClass, PartyInfo);
}

void UVictimsChatManager::SetPlayerName_Implementation(const FString& InNewName)
{
	if (InNewName.IsEmpty())
	{
		UE_CLOG(bIsDebuggingMode, LogTemp, Error, L"%hs() - In new player name is empty.", __FUNCTION__);
		return;
	}
	FString NewName{ InNewName };
	for (FString Elem : PlayerNameDisallowedChars)
	{
		if (!NewName.Contains(Elem))
		{
			continue;
		}
		NewName = NewName.Replace(*Elem, L"");
		OnSetInvalidPlayerName.Broadcast(InNewName);
	}

	// bool bIsDuplicates{false};

	for (const TTuple<TObjectPtr<AActor>, TObjectPtr<UVictimsChatManager>> Elem :
		GetRegisteredChatManagers())
	{
		if (Elem.Value->GetPlayerName() == NewName)
		{
			// bIsDuplicates = true;
			do
			{
				OnSetInvalidPlayerName.Broadcast(InNewName);
				NewName += FString("1");
			} while (Elem.Value->GetPlayerName() == NewName);
		}
	}

	PlayerName = NewName;
}

void UVictimsChatManager::ReceiveMsg(const FVictims_MsgInfo& InMsgInfo)
{
	MsgInfo = InMsgInfo;

	Server_ProcessMsg(InMsgInfo);
}

const FChatPartyInfo& UVictimsChatManager::GetPartyInfo()
{
	return PartyInfo;
}

void UVictimsChatManager::Server_LeaveParty_Implementation()
{
	// No party
	if (!PartyInfo.PartyLeader)
	{
		return;
	}

	UVictimsChatManager* LeaderChatManager{ GetChatManagerFromActor(PartyInfo.PartyLeader) };

	if (IsLeaderOfTheParty())
	{
		Server_BreakParty();

		//////////////////////////////////파티 컴포넌트 호출///////////////////////////////////////////////////////////////////////
		if (ownerCharacter)
		{
			PartyName = ownerCharacter->GetName();
		
			ownerPartyComponent->ServerRPC_preBreakParty_Implementation();
		}
		//생성 시점이 안 맞아서 널포인터가 뜨니까, 다시 캐싱 해주자
		else
		{
			AVICTIMSPlayerController* OwnerController = Cast<AVICTIMSPlayerController>(GetOwner());
			if (OwnerController)
			{
				// 소유자의 UPartyComponent를 캐싱!!!!!
				UPartyComponent* OwnerPartyComponent = OwnerController->FindComponentByClass<UPartyComponent>();
				if (OwnerPartyComponent)
				{
					ownerPartyComponent = OwnerPartyComponent;
		
		
					APawn* ownerPawn = OwnerController->GetPawn();
					if (ownerPawn)
					{
		
						AVICTIMSCharacter* ownerVictimsChar = Cast<AVICTIMSCharacter>(ownerPawn);
						if (ownerVictimsChar)
						{
							ownerCharacter = ownerVictimsChar;
		
		
							if (ownerCharacter)
							{
								ownerPartyComponent->ServerRPC_preBreakParty_Implementation();
							}
						}
		
		
		
					}
		
		
				}
			}
		}



	}
	else if (LeaderChatManager)
	{
		LeaderChatManager->ExpelPartyMember(GetOwner());
	}

	FVictims_MsgInfo NewLog{ FVictims_MsgInfo(FDateTime::Now(),
	ArrM_CmdChannels[FString("LeaveParty")].DescOfChannel,
	ArrM_CmdChannels[FString("LeaveParty")].ChannelToTransmitExplanation, false) };

	Client_ReceivedChat(NewLog);

	NewLog.Msg = GetPlayerName() + FString(" ") + NewLog.Msg;

	if (LeaderChatManager)
	{
		LeaderChatManager->Client_ReceivedChat(NewLog);
	}
	for (TObjectPtr<APlayerState> Elem : GetPartyInfo().PartyMembers)
	{
		if (UVictimsChatManager * MemberChatManager{ GetChatManagerFromActor(Elem) })
		{
			MemberChatManager->Client_ReceivedChat(NewLog);
		}
	}
}

void UVictimsChatManager::Server_SendWhisperMsg_Implementation(const FString& InTargetID, const FVictims_MsgInfo& InMsgInfo)
{
	TMap/*<TObjectPtr<AActor>, TObjectPtr<UGmAC_ChatManager>>*/ RegisteredChatManagers{
		GetRegisteredChatManagers()
	};

	if (const TObjectPtr<UVictimsChatManager>*TargetChatManagerPtr{ RegisteredChatManagers.Find(GetOwner()) })
	{
		UVictimsChatManager* TargetChatManager{ TargetChatManagerPtr->Get() };
		for (const TTuple<TObjectPtr<AActor>, TObjectPtr<UVictimsChatManager>> Elem : RegisteredChatManagers)
		{
			if (Elem.Value->GetPlayerName() != InTargetID || Elem.Value->GetWorld() != GetWorld())
			{
				continue;
			}

			FVictims_MsgInfo NewMsgInfo{ InMsgInfo };
			NewMsgInfo.Recipient = InTargetID;
			NewMsgInfo.SenderName = TargetChatManager->GetPlayerName();
			NewMsgInfo.bIsSender = false;

			Elem.Value->Client_ReceivedChat(NewMsgInfo);

			NewMsgInfo.bIsSender = true;
			TargetChatManager->Client_ReceivedChat(NewMsgInfo);
		}
	}
}

void UVictimsChatManager::Server_BreakParty_Implementation()
{
	if (!IsLeaderOfTheParty())
	{
		UE_CLOG(bIsDebuggingMode, LogTemp, Error, L"%s is not Leader of the party.", *GetPlayerName());
		return;
	}

	const FVictims_MsgInfo BreakPartyMsg{ FVictims_MsgInfo(FDateTime::Now(),
				FString("The party disbanded."), EChatChannelType::SystemLog, false) };

	for (APlayerState* MemberPS : PartyInfo.PartyMembers)
	{
		if (UVictimsChatManager * MemberChatManager{ GetChatManagerFromActor(MemberPS) })
		{
			MemberChatManager->Client_ReceivedChat(BreakPartyMsg);
			MemberChatManager->PartyInfo = nullptr;
		}
	}

	if (UVictimsChatManager * LeaderChatManager{ GetChatManagerFromActor(PartyInfo.PartyLeader) })
	{
		LeaderChatManager->Client_ReceivedChat(BreakPartyMsg);
		PartyInfo = nullptr;
	}
}

void UVictimsChatManager::Server_SendPartyInvitationsByName_Implementation(const FString& InTargetPlayerName)
{
	if (!IsLeaderOfTheParty())
	{
		Client_ReceivedChat(FVictims_MsgInfo(FDateTime::Now(), L"You are not the leader of the party.",
			EChatChannelType::Game, false));
		return;
	}
	const UVictims_ChatSubsystem* ChatSubsystem{ GetChatSubsystem() };
	if (!ChatSubsystem)
	{
		return;
	}

	if (UVictimsChatManager* TargetChatManager{ ChatSubsystem->GetChatManagerByPlayerName(InTargetPlayerName) };
		TargetChatManager && TargetChatManager != this)
	{
		if (PartyInfo.PartyMembers.Contains(GetPSFromActor(TargetChatManager->GetOwner())))
		{
			//Person A is already in the current party.
			Client_ReceivedChat(FVictims_MsgInfo(FDateTime::Now(),
				TargetChatManager->GetPlayerName() + FString(" is already in the current party."),
				EChatChannelType::Game, false));
		}
		TargetChatManager->SimpleLeaderID = GetPlayerName();
		TargetChatManager->Client_CreateInvitePartyWidget();
	}
}

void UVictimsChatManager::ExpelPartyMember_Implementation(AActor* InTargetMember)
{
	if (!InTargetMember)
	{
		UE_CLOG(bIsDebuggingMode, LogTemp, Error, L"The target being kicked out of the party is invalid.");
		return;
	}

	if (!IsLeaderOfTheParty())
	{
		return;
	}


	if (UVictimsChatManager * TargetChatManager{ GetChatManagerFromActor(InTargetMember) })
	{
		TargetChatManager->PartyInfo = nullptr;
	}
	PartyInfo.PartyMembers.Remove(GetPSFromActor(InTargetMember));

}
//여기서 아주 배열로 받아버려..?
void UVictimsChatManager::Server_AddNewPartyMember_Implementation(AActor* InNewPartyMember)
{
	if (!InNewPartyMember)
	{
		UE_CLOG(bIsDebuggingMode, LogTemp, Error, L"New party member is not valid.");
		return;
	}

	if (!IsLeaderOfTheParty())
	{
		return;
	}

	if (APlayerState * TargetPS{ GetPSFromActor(InNewPartyMember) })
	{
		// UE_LOG(LogTemp, Error, L"TargetPS is valid.");
		if (UVictimsChatManager * NewMembersChatManager{ TargetPS->GetOwningController()->FindComponentByClass<UVictimsChatManager>() })
		{
			// UE_LOG(LogTemp, Error, L"NewMembersChatManager is valid.");
			if (PartyInfo.PartyMembers.Contains(InNewPartyMember) || NewMembersChatManager->PartyInfo.PartyLeader)
			{
				//@TODO Should we find out whether they belong to the same party or different parties?
				UE_CLOG(bIsDebuggingMode, LogTemp, Error, L"New party member is already in the party.");
				return;
			}

			// UE_LOG(LogTemp, Error, L"Add Party Member()");
			PartyInfo.PartyMembers.Add(TargetPS);

			//@TODO Should I move it inside the FGmPartyInfo structure?
			NewMembersChatManager->PartyInfo = PartyInfo;

			const FVictims_MsgInfo NewMsgInfo{ FVictims_MsgInfo(FDateTime::Now(),
				NewMembersChatManager->GetPlayerName() + FString(" has joined the party."),
				EChatChannelType::Party, true) };

			// Leader notify
			if (UVictimsChatManager * LeaderChatManager{ GetChatManagerFromActor(PartyInfo.PartyLeader) })
			{
				LeaderChatManager->Client_ReceivedChat(NewMsgInfo);
			}

			// System notification for party joining
			for (APlayerState* Members : PartyInfo.PartyMembers)
			{
				if (UVictimsChatManager * MemberChatManager{ GetChatManagerFromActor(Members) })
				{
					MemberChatManager->Client_ReceivedChat(NewMsgInfo);
				}
			}
			if (!NewMembersChatManager->SimpleLeaderID.IsEmpty())
			{
				NewMembersChatManager->SimpleLeaderID = L"";
			}
		}
	}
}

void UVictimsChatManager::Server_CreateNewParty_Implementation()
{
	// UE_LOG(LogTemp, Error, L"Server_CreateParty() is run.");
	if (PartyInfo.PartyLeader)
	{
		UE_CLOG(bIsDebuggingMode, LogTemp, Error, L"Joined party already exists.");
		return;
	}

	if (APlayerState * TargetPS{ GetPSFromActor(GetOwner()) })
	{
		UE_CLOG(bIsDebuggingMode, LogTemp, Error, L"You have successfully created a party.");
		PartyInfo.PartyLeader = TargetPS;

		Client_ReceivedChat(FVictims_MsgInfo(FDateTime::Now(),
			ArrM_CmdChannels[FString("CreateParty")].DescOfChannel,
			ArrM_CmdChannels[FString("CreateParty")].ChannelToTransmitExplanation, false));

		//////////////////////////////////파티 컴포넌트 호출///////////////////////////////////////////////////////////////////////
		if (ownerCharacter)
		{
			PartyName = ownerCharacter->GetName();
		
			ownerPartyComponent->ServerRPC_CreateNewParty(ownerCharacter, PartyName);
		}
		//생성 시점이 안 맞아서 널포인터가 뜨니까, 다시 캐싱 해주자
		else
		{
			AVICTIMSPlayerController* OwnerController = Cast<AVICTIMSPlayerController>(GetOwner());
			if (OwnerController)
			{
				// 소유자의 UPartyComponent를 캐싱!!!!!
				UPartyComponent* OwnerPartyComponent = OwnerController->FindComponentByClass<UPartyComponent>();
				if (OwnerPartyComponent)
				{
					ownerPartyComponent = OwnerPartyComponent;
		
		
					APawn* ownerPawn = OwnerController->GetPawn();
					if (ownerPawn)
					{
		
						AVICTIMSCharacter* ownerVictimsChar = Cast<AVICTIMSCharacter>(ownerPawn);
						if (ownerVictimsChar)
						{
							ownerCharacter = ownerVictimsChar;
		
							
							if (ownerCharacter)
							{ 
								PartyName = ownerCharacter->GetName();
		
								ownerPartyComponent->ServerRPC_CreateNewParty_Implementation(ownerCharacter, PartyName);
							}
						}
		
		
						
					}
		
					
				}
			}
		}
		
	}
	else
	{
		UE_CLOG(bIsDebuggingMode, LogTemp, Error, L"Can not created new party.");
	}
}

void UVictimsChatManager::Server_ProcessMsg_Implementation(const FVictims_MsgInfo& InMsgInfo)
{
	MsgInfo = InMsgInfo;

	if (UVictims_ChatSubsystem * ChatSubsystem{ GetChatSubsystem() })
	{
		switch (MsgInfo.Channel)
		{
		case EChatChannelType::None:
			break;
		case EChatChannelType::Global:
			ChatSubsystem->ReceivedGlobalChat(GetOwner(), InMsgInfo);
			break;
//		case EChatChannelType::Friend:
//			break;
		case EChatChannelType::Party:
			Server_ReceivedPartyChat();
			break;
		case EChatChannelType::Private:
			Server_ReceivedPrivateChat();
			break;
		case EChatChannelType::Announcement:
			break;
		case EChatChannelType::Game:
			Server_ReceivedGameChat();
			break;
		case EChatChannelType::SystemLog:
			Server_ReceivedSystemLogChat();
			break;
		}
	}

	// UE_LOG(LogTemp, Error, L"Server_ProcessMsg() - %s.", *InMsgInfo.Msg);
}

void UVictimsChatManager::Client_ReceivedChat_Implementation(const FVictims_MsgInfo& InMsgInfo)
{
	MsgInfo = InMsgInfo;

	for (UWidget* Elem : ChatWidgetRef->HB_Tabs->GetAllChildren())
	{
		if (UVictims_ContainerTab * ContainerTab{ Cast<UVictims_ContainerTab>(Elem) })
		{
			// Check if the required channel is correct
			if (ContainerTab->ArrM_FilteringSettingsForEachChannel.Contains(static_cast<uint8>(InMsgInfo.Channel)) &&
				ContainerTab->ArrM_FilteringSettingsForEachChannel[static_cast<uint8>(InMsgInfo.Channel)])
			{
				if (MsgWidgetClassRef)
				{
					if (UVictims_ChatMsg * Loc_MsgToAdd{ CreateWidget<UVictims_ChatMsg>(
						Cast<APlayerController>(GetOwner()), MsgWidgetClassRef) })
					{
						MsgInfo.bShowTimeStamp = ChatWidgetRef->bShowTimeStamps;
						MsgInfo.MsgTime = FDateTime::Now();

						if (ArrM_GeneralInfoByChannel.Contains(MsgInfo.Channel))
						{
							// Set msg color by channel
							MsgInfo.MsgColor = ArrM_GeneralInfoByChannel[MsgInfo.Channel].TextColor;

							// Set msg font info
							if (!MsgInfo.MsgFontInfo.FontObject)
							{
								MsgInfo.MsgFontInfo = ArrM_GeneralInfoByChannel[MsgInfo.Channel].TextFontInfo;
							}
						}

						Loc_MsgToAdd->MsgInfo = MsgInfo;

						if (ChatWidgetRef->CurrentOpenedTab == ContainerTab)
						{
							ChatWidgetRef->UpdateMsgInChatBox(Loc_MsgToAdd);
						}
						ContainerTab->Msges.Add(Loc_MsgToAdd);
					}
				}
				else
				{
					UE_LOG(LogTemp, Error, L"Message widget class reference is not valid.");
				}
			}
		}
	}
}

void UVictimsChatManager::Server_ReceivedGameChat_Implementation()
{
	Client_ReceivedChat(MsgInfo);
}

void UVictimsChatManager::Server_ReceivedPartyChat_Implementation()
{
	if (PartyInfo.PartyLeader)
	{
		// Party leader
		GetChatManagerFromActor(PartyInfo.PartyLeader)->Client_ReceivedChat(MsgInfo);

		// Party members
		for (APlayerState* Elem : PartyInfo.PartyMembers)
		{
			if (UVictimsChatManager * ElemChatManager{ GetChatManagerFromActor(Elem) })
			{
				ElemChatManager->Client_ReceivedChat(MsgInfo);
			}
		}
	}
	else
	{
		Client_ReceivedChat(FVictims_MsgInfo(FDateTime::Now(), FString(
			"There are currently no registered parties."), EChatChannelType::SystemLog, false));
	}
}

void UVictimsChatManager::Server_ReceivedSystemLogChat_Implementation()
{
	Client_ReceivedChat(MsgInfo);
}

void UVictimsChatManager::Server_ReceivedPrivateChat_Implementation()
{
	// UE_LOG(LogTemp, Error, L"Received private msg - %s.", *GetNameSafe(Cast<AController>(GetOwner())->GetPawn()));
	if (const UWorld * World{ GetWorld() })
	{
		for (TActorIterator<APlayerController> ActorItr(World); ActorItr; ++ActorItr)
		{
			if (const APlayerController * TargetPC{ *ActorItr })
			{
				if (UVictimsChatManager * TargetChatManager{ TargetPC->FindComponentByClass<UVictimsChatManager>() })
				{
					if (TargetChatManager->GetPlayerName() == MsgInfo.Recipient)
					{
						TargetChatManager->Client_ReceivedChat(MsgInfo);

						FVictims_MsgInfo NewMsgInfo{ MsgInfo };
						NewMsgInfo.bIsSender = !NewMsgInfo.bIsSender;
						Client_ReceivedChat(NewMsgInfo);
						break;
					}
				}
			}
		}
	}
}

void UVictimsChatManager::Client_CreateInvitePartyWidget_Implementation()
{
	if (UVictims_FloatingWidget * WhisperWidget{ CreateWidget<UVictims_FloatingWidget>(
	Cast<APlayerController>(GetOwner()), WhisperWidgetClassRef) })
	{
		WhisperWidget->WidgetTitle = FText::FromString("Party Invitation");
		WhisperWidget->WidgetDescription =
			FText::FromString("Would you like to accept the party invitation?");
		WhisperWidget->bHasCancelBtn = true;
		WhisperWidget->bNeedEditableTextBox = false;
		WhisperWidget->AcceptBtnText = FText::FromString(FString("Accept"));
		WhisperWidget->CancelBtnText = FText::FromString(FString("Cancel"));

		WhisperWidget->GetOnClickedAcceptOrCancelBtnDel(true).BindLambda([WhisperWidget](const AController* InOwningC)->void
			{
				if (const UVictims_ChatSubsystem* ChatSubsystem{ WhisperWidget->GetGameInstance()->GetSubsystem<UVictims_ChatSubsystem>() })
				{
					ChatSubsystem->AddNewPartyMember(InOwningC);
				}
			});

		WhisperWidget->AddToPlayerScreen();
	}
}

void UVictimsChatManager::Server_ApplyForPartyEntry_Implementation()
{
	if (const UVictims_ChatSubsystem * ChatSubsystem{ GetChatSubsystem() })
	{
		for (const TTuple<TObjectPtr<AActor>, TObjectPtr<UVictimsChatManager>> Elem : ChatSubsystem->GetRegisteredChatManagers())
		{
			if (!SimpleLeaderID.IsEmpty() && Elem.Value->GetPlayerName() == SimpleLeaderID)
			{
				Elem.Value->Server_AddNewPartyMember(GetOwner());
				SimpleLeaderID = FString("");
			}
		}
	}
}

void UVictimsChatManager::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* OwnerPC{ GetOwner<APlayerController>() }; OwnerPC && OwnerPC->IsLocalPlayerController())
	{
		ChatWidgetRef = CreateWidget<UVictims_ChatBox>(OwnerPC, ChatWidgetClassRef);

		if (!ChatWidgetRef || !ChatWidgetClassRef)
		{
			UE_LOG(LogTemp, Error, L"Chat widget reference is not valid.");
			return;
		}

		ChatWidgetRef->ContainerTabClassRef = ContainerTabClassRef;
		ChatWidgetRef->AddToPlayerScreen();

		if (UInputComponent * InputComp{ OwnerPC->InputComponent })
		{
			InputComp->BindKey(EKeys::Enter, IE_Pressed, this, &ThisClass::GmOnPressedEnterFunc);
			InputComp->BindKey(EKeys::Slash, IE_Pressed, this, &ThisClass::GmOnPressedSlashFunc);
			InputComp->BindKey(WhisperShortcutKey, IE_Pressed, this, &ThisClass::GmOnPressedWhisperKeyFunc);
		}
	}

	//@TODO Make unique player name logic, scheduled to be removed
	if (const UWorld * World{ GetWorld() })
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([this, World]()->void
			{
				if (PlayerName.IsEmpty())
				{
					SetPlayerName(std::to_string(GetUniqueID()).c_str());
				}
				GetChatSubsystem()->RegisterChatManager(this);
			}));
	}

	////partyComponent캐싱
	//AVICTIMSPlayerController* OwnerController = Cast<AVICTIMSPlayerController>(GetOwner());
	//if (OwnerController)
	//{
	//	// 소유자의 UPartyComponent를 캐싱!!!!!
	//	UPartyComponent* OwnerPartyComponent = OwnerController->FindComponentByClass<UPartyComponent>();
	//	if (OwnerPartyComponent)
	//	{
	//		ownerPartyComponent=OwnerPartyComponent;
	//
	//
	//		APawn*ownerPawn = OwnerController->GetPawn();
	//		if (ownerPawn)
	//		{
	//
	//			AVICTIMSCharacter* ownerVictimsChar=Cast<AVICTIMSCharacter>(ownerPawn);
	//			if (ownerVictimsChar)
	//			{
	//				ownerCharacter=ownerVictimsChar;
	//			}
	//
	//		}
	//	}
	//}
}
void UVictimsChatManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetChatSubsystem()->UnregisterChatManager(this);

	Super::EndPlay(EndPlayReason);
}

void UVictimsChatManager::GmOnPressedEnterFunc()
{
	UE_CLOG(bIsDebuggingMode, LogTemp, Error, L"GmOnPressedEnterFunc");
	if (ChatWidgetRef)
	{
		ChatWidgetRef->FocusChat(false, false);
		// ChatWidgetRef->UpdateTextColor();
	}
}

void UVictimsChatManager::GmOnPressedSlashFunc()
{
	UE_CLOG(bIsDebuggingMode, LogTemp, Error, L"GmOnPressedSlashFunc");
	if (ChatWidgetRef)
	{
		ChatWidgetRef->FocusChat(true, false);
		// ChatWidgetRef->UpdateTextColor();
	}
}

void UVictimsChatManager::GmOnPressedWhisperKeyFunc()
{
	if (WhisperWidgetClassRef && ChatWidgetRef && !ChatWidgetRef->MsgTextBox->HasKeyboardFocus())
	{
		if (UVictims_FloatingWidget * WhisperWidget{ CreateWidget<UVictims_FloatingWidget>(
			static_cast<APlayerController*>(GetOwner()), WhisperWidgetClassRef) })
		{
			WhisperWidget->WidgetTitle = FText::FromString("Whisper");
			WhisperWidget->WidgetDescription =
				FText::FromString("Enter the name of the person you want to send a whisper to.");
			WhisperWidget->bNeedEditableTextBox = true;
			WhisperWidget->AcceptBtnText = FText::FromString(FString("Accept"));

			WhisperWidget->GetOnClickedAcceptOrCancelBtnDel(true).BindLambda(
				[this, WhisperWidget](const AController* InOwningC)->void
				{
					// Update channel
					if (const FString CurrentTargetID{ WhisperWidget->ETB_WhisperInput->GetText().ToString() };
						!CurrentTargetID.IsEmpty())
					{
						if (ChatWidgetRef)
						{
							ChatWidgetRef->UpdateCurrentChannel(EChatChannelType::Private, CurrentTargetID);
							ChatWidgetRef->UpdateTextColor();
							ChatWidgetRef->UpdateTypingInChannel();
							ChatWidgetRef->FocusChat(false, false);
						}
					}
				});

			WhisperWidget->AddToPlayerScreen();
			WhisperWidget->ETB_WhisperInput->SetKeyboardFocus();
		}
	}
	UE_CLOG(bIsDebuggingMode && !WhisperWidgetClassRef, LogTemp, Error, L"Whisper server widget class ref is not valid.");
}

UVictims_ChatSubsystem* UVictimsChatManager::GetChatSubsystem() const
{
	if (const UWorld* World{ GetWorld() };
		World && World->GetGameInstance())
	{
		return World->GetGameInstance()->GetSubsystem<UVictims_ChatSubsystem>();
	}
	return nullptr;
}

TMap<TObjectPtr<AActor>, TObjectPtr<UVictimsChatManager>> UVictimsChatManager::GetRegisteredChatManagers() const
{
	TMap<TObjectPtr<AActor>, TObjectPtr<UVictimsChatManager>> ReturnValue;
	if (const UVictims_ChatSubsystem * ChatSubsystem{ GetChatSubsystem() })
	{
		ReturnValue = ChatSubsystem->GetRegisteredChatManagers();
	}
	return ReturnValue;
}

APlayerState* UVictimsChatManager::GetPSFromActor(AActor* InTarget) const
{
	if (!InTarget)
	{
		return nullptr;
	}
	if (APlayerState * TargetIsPS{ Cast<APlayerState>(InTarget) })
	{
		return TargetIsPS;
	}
	if (const APawn * TargetIsPawn{ Cast<APawn>(InTarget) })
	{
		return TargetIsPawn->GetPlayerState();
	}
	if (const AController * TargetIsController{ Cast<AController>(InTarget) })
	{
		return TargetIsController->PlayerState;
	}

	UE_CLOG(bIsDebuggingMode, LogTemp, Error, L"The target is not APawn, AController, or APlayerState.");
	return nullptr;
}

UVictimsChatManager* UVictimsChatManager::GetChatManagerFromActor(AActor* InTarget) const
{
	const APlayerState* TargetPS{ GetPSFromActor(InTarget) };
	if (!TargetPS)
	{
		return nullptr;
	}
	return TargetPS->GetOwningController()->FindComponentByClass<UVictimsChatManager>();
}

bool UVictimsChatManager::IsLeaderOfTheParty() const
{
	if (!PartyInfo.PartyLeader)
	{
		UE_CLOG(bIsDebuggingMode, LogTemp, Error, L"The player currently has no parties.");
		return false;
	}

	if (const AController* CurrentC{ GetOwner<AController>() }; PartyInfo.PartyLeader != CurrentC->PlayerState)
	{
		UE_CLOG(bIsDebuggingMode, LogTemp, Error, L"Current player is not a party leader.");
		UE_CLOG(bIsDebuggingMode, LogTemp, Error, L"Current party leader is %s.", *GetNameSafe(PartyInfo.PartyLeader));
		UE_CLOG(bIsDebuggingMode, LogTemp, Error, L"Current player is %s.", *GetNameSafe(CurrentC->PlayerState));
		if (const AController * CurrentLeaderC{ PartyInfo.PartyLeader->GetPlayerController() })
		{
			UE_CLOG(bIsDebuggingMode, LogTemp, Error, L"Current leader is %s.", *CurrentLeaderC->FindComponentByClass<UVictimsChatManager>()->PlayerName);
		}
		UE_CLOG(bIsDebuggingMode, LogTemp, Error, L"Error player is %s.", *CurrentC->FindComponentByClass<UVictimsChatManager>()->PlayerName);
		return false;
	}

	return true;
}