// Fill out your copyright notice in the Description page of Project Settings.


#include "Victims_ChatSubsystem.h"


#include "VictimsChatManager.h"


UVictims_ChatSubsystem::UVictims_ChatSubsystem()
{
}

void UVictims_ChatSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UVictims_ChatSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

bool UVictims_ChatSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return Super::ShouldCreateSubsystem(Outer);
}

void UVictims_ChatSubsystem::RegisterChatManager(UVictimsChatManager* InTarget)
{
	check(InTarget);

	RegisteredChatManagers.FindOrAdd(InTarget->GetOwner(), InTarget);

	UE_CLOG(InTarget->bIsDebuggingMode, LogTemp, Error, L"%hs() - Registered %s.", __FUNCTION__, *InTarget->GetPlayerName());
}

void UVictims_ChatSubsystem::UnregisterChatManager(const UVictimsChatManager* InTarget)
{
	check(InTarget);

	RegisteredChatManagers.Remove(InTarget->GetOwner());

	UE_CLOG(InTarget->bIsDebuggingMode, LogTemp, Error, L"%hs() - Unregistered %s.", __FUNCTION__, *InTarget->GetPlayerName());
}

void UVictims_ChatSubsystem::Internal_ReceiveMsgToAll(const FVictims_MsgInfo& InMsgInfo) const
{
	for (const TTuple<TObjectPtr<AActor>, TObjectPtr<UVictimsChatManager>> Elem : RegisteredChatManagers)
	{
		Elem.Value->Client_ReceivedChat(InMsgInfo);
	}
}

void UVictims_ChatSubsystem::ReceiveMsg(const AActor* InManagerOwner, const FVictims_MsgInfo& InMsgInfo) const
{
	if (!InManagerOwner)
	{
		UE_LOG(LogTemp, Error, L"%hs() - InManagerOwner is not valid.", __FUNCTION__);
		return;
	}
	if (const TObjectPtr<UVictimsChatManager>*TargetChatManager{ RegisteredChatManagers.Find(InManagerOwner) })
	{
		TargetChatManager->Get()->ReceiveMsg(InMsgInfo);
	}
}

void UVictims_ChatSubsystem::ReceivedGlobalChat(const AActor* InManagerOwner, const FVictims_MsgInfo& InMsgInfo)
{
	if (!InManagerOwner)
	{
		UE_LOG(LogTemp, Error, L"%hs() - InManagerOwner is not valid.", __FUNCTION__);
		return;
	}
	for (const TTuple<TObjectPtr<AActor>, TObjectPtr<UVictimsChatManager>> Elem : RegisteredChatManagers)
	{
		if (Elem.Key->GetWorld() != InManagerOwner->GetWorld())
		{
			continue;
		}
		// UE_LOG(LogTemp, Error, L"Player name is %s.", *Elem->PlayerName);
		Elem.Value->Client_ReceivedChat(InMsgInfo);
	}
}

void UVictims_ChatSubsystem::SendWhisperMsg(const AActor* InManagerOwner, const FString& InTargetID, const FVictims_MsgInfo& InMsgInfo)
{
	if (!InManagerOwner)
	{
		UE_LOG(LogTemp, Error, L"%hs() - InManagerOwner is not valid.", __FUNCTION__);
		return;
	}
	if (const TObjectPtr<UVictimsChatManager>*TargetChatManagerPtr{ RegisteredChatManagers.Find(InManagerOwner) })
	{
		TargetChatManagerPtr->Get()->Server_SendWhisperMsg(InTargetID, InMsgInfo);
	}
}

void UVictims_ChatSubsystem::SendPartyInvitationsByName(const AActor* InManagerOwner, const FString& InTargetPlayerName) const
{
	if (UVictimsChatManager * ChatManager{ GetChatManagerByOwningActor(InManagerOwner) })
	{
		ChatManager->Server_SendPartyInvitationsByName(InTargetPlayerName);
	}
}

void UVictims_ChatSubsystem::CreateNewParty(const AActor* InManagerOwner) const
{
	if (UVictimsChatManager * TargetChatManager{ GetChatManagerByOwningActor(InManagerOwner) })
	{
		TargetChatManager->Server_CreateNewParty();
	}
}

void UVictims_ChatSubsystem::BreakParty(const AActor* InManagerOwner) const
{
	if (UVictimsChatManager * ChatManager{ GetChatManagerByOwningActor(InManagerOwner) })
	{
		ChatManager->Server_BreakParty();
	}
}

void UVictims_ChatSubsystem::LeaveParty(const AActor* InManagerOwner) const
{
	if (UVictimsChatManager * ChatManager{ GetChatManagerByOwningActor(InManagerOwner) })
	{
		ChatManager->Server_LeaveParty();
	}
}

void UVictims_ChatSubsystem::AddNewPartyMember(const AActor* InManagerOwner) const
{
	if (UVictimsChatManager * ChatManager{ GetChatManagerByOwningActor(InManagerOwner) })
	{
		ChatManager->Server_ApplyForPartyEntry();
	}
}

UVictimsChatManager* UVictims_ChatSubsystem::GetChatManagerByPlayerName(const FString& InTargetPlayerName) const
{
	for (const TTuple<TObjectPtr<AActor>, TObjectPtr<UVictimsChatManager>> Elem : RegisteredChatManagers)
	{
		if (Elem.Value->GetPlayerName() != InTargetPlayerName)
		{
			continue;
		}
		return Elem.Value;
	}
	return nullptr;
}

UVictimsChatManager* UVictims_ChatSubsystem::GetChatManagerByOwningActor(const AActor* InOwningActor) const
{
	return GetRegisteredChatManagers().Contains(InOwningActor) ? GetRegisteredChatManagers()[InOwningActor]
		: nullptr;
}

FString UVictims_ChatSubsystem::GetPlayerNameByOwningActor(const AActor* InOwningActor) const
{
	if (const UVictimsChatManager * ChatManager{ GetChatManagerByOwningActor(InOwningActor) })
	{
		return ChatManager->GetPlayerName();
	}
	return L"";
}

TMap<TObjectPtr<AActor>, TObjectPtr<UVictimsChatManager>> UVictims_ChatSubsystem::GetRegisteredChatManagers() const
{
	return RegisteredChatManagers;
}

#if WITH_EDITOR
bool UVictims_ChatSubsystem::GetIsDebuggingModeFromOwningActor(const AActor* InOwningActor) const
{
	if (const UVictimsChatManager * ChatManager{ GetChatManagerByOwningActor(InOwningActor) })
	{
		return ChatManager->bIsDebuggingMode;
	}
	return false;
}
#endif

