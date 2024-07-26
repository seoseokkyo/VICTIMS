// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Victims_ChatSubsystem.generated.h"

/**
 * 
 */
 struct FVictims_MsgInfo;
 class UVictimsChatManager;

UCLASS()
class VICTIMS_API UVictims_ChatSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	UVictims_ChatSubsystem();

	//~USubsystem interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	//~End of USubsystem interface

	/** Register on Chat Manager with global system. */
	void RegisterChatManager(UVictimsChatManager* InTarget);

	/** Removes on Chat Manager from global system. */
	void UnregisterChatManager(const UVictimsChatManager* InTarget);

	void Internal_ReceiveMsgToAll(const FVictims_MsgInfo& InMsgInfo) const;

	void ReceiveMsg(const AActor* InManagerOwner, const FVictims_MsgInfo& InMsgInfo) const;

	void ReceivedGlobalChat(const AActor* InManagerOwner, const FVictims_MsgInfo& InMsgInfo);
	void SendWhisperMsg(const AActor* InManagerOwner, const FString& InTargetID, const FVictims_MsgInfo& InMsgInfo);
	void SendPartyInvitationsByName(const AActor* InManagerOwner, const FString& InTargetPlayerName) const;
	void CreateNewParty(const AActor* InManagerOwner) const;
	void BreakParty(const AActor* InManagerOwner) const;
	void LeaveParty(const AActor* InManagerOwner) const;
	void AddNewPartyMember(const AActor* InManagerOwner) const;

	UVictimsChatManager* GetChatManagerByPlayerName(const FString& InTargetPlayerName) const;
	UVictimsChatManager* GetChatManagerByOwningActor(const AActor* InOwningActor) const;
	FString GetPlayerNameByOwningActor(const AActor* InOwningActor) const;
	TMap<TObjectPtr<AActor>, TObjectPtr<UVictimsChatManager>> GetRegisteredChatManagers() const;
	// FString GetCmdPrefixByOwningActor(const AActor* InOwningActor) const;
	// TMap<FString, struct FGmCmdDescOfChannel> GetCmdChannelsByOwningActor(const AActor* InOwningActor) const;
	// TMap<enum class EGmChannelType, struct FGmGeneralInfo> GetGeneralInfoByChannelByOwningActor(const AActor* InOwningActor) const;
	// TMap<EGmChannelType, bool> GetFilteringSettingsForEachChannelByOwningActor(const AActor* InOwningActor) const;

#if WITH_EDITOR
	bool GetIsDebuggingModeFromOwningActor(const AActor* InOwningActor) const;
#endif

private:

	TMap<TObjectPtr<AActor>, TObjectPtr<UVictimsChatManager>> RegisteredChatManagers;

};
