// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"
#include "Victims_ChatBox.h"
#include "ChatChannelTypeDef.h"

#include "VICTIMSCharacter.h"
#include "VictimsChatManager.generated.h"

class UVictims_ChatMsg;
struct FVictims_MsgInfo;
class UVictims_ChatBox;
class APlayerState;
class UVictims_FloatingWidget;
class UPartyComponent;

USTRUCT(BlueprintType)
struct FChatPartyInfo
{
	GENERATED_BODY()

	FChatPartyInfo()
		:
	PartyLeader(nullptr)
	{}

	FChatPartyInfo(APlayerState* InNewPartyLeader)
		:
	PartyLeader(InNewPartyLeader)
	{}
public:
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<APlayerState> PartyLeader;


	UPROPERTY(BlueprintReadWrite)
	TArray<TObjectPtr<APlayerState>> PartyMembers;



};

UCLASS(ClassGroup = (MultiChatSystem), meta = (BlueprintSpawnableComponent))
class VICTIMS_API UVictimsChatManager : public UActorComponent
{
	GENERATED_BODY()

public:

	UVictimsChatManager(const FObjectInitializer& ObjectInitializer);


	//PartyComponent Ä³½Ì
	UPROPERTY(BlueprintReadWrite, Replicated)
	UPartyComponent* ownerPartyComponent;
	
	UPROPERTY(BlueprintReadWrite, Replicated)
	AVICTIMSCharacter* ownerCharacter;
	
	UPROPERTY(BlueprintReadWrite, Replicated)
	FString PartyName;
	//

	UPROPERTY(EditDefaultsOnly, Category = "Init Settings|Short cut")
	struct FKey WhisperShortcutKey{EKeys::J};

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Init Settings|Ref")
	TSubclassOf<UVictims_ChatBox> ChatWidgetClassRef;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Init Settings|Ref")
	TSubclassOf<UVictims_FloatingWidget> WhisperWidgetClassRef;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Init Settings|Ref")
	TSubclassOf<UVictims_ChatMsg> MsgWidgetClassRef;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Init Settings|Ref")
	TSubclassOf<UVictims_ContainerTab> ContainerTabClassRef;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Debugging")
	bool bIsDebuggingMode{false};

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Init Settings|Command")
	FString CmdPrefix{FString("/")};

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Init Settings", DisplayName = "Command By Channel")
	TMap<FString, FChatCmdDescOfChannel> ArrM_CmdChannels;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Init Settings|Disallowed chars in player name", DisplayName = "")
	TArray<FString> PlayerNameDisallowedChars;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Init Settings", DisplayName = "Color By Channel")
	TMap<EChatChannelType, FChatGeneralInfo> ArrM_GeneralInfoByChannel;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Init Settings", DisplayName = "Allowed Channel Init Settings")
	TMap<EChatChannelType, bool> ArrM_FilteringSettingsForEachChannel;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "GmDMChatSystem")
	FORCEINLINE FString GetPlayerName() const
	{
		return PlayerName;
	}

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "GmDMChatSystem", DisplayName = "Set Player Name")
	void SetPlayerName(UPARAM(DisplayName = "New Player Name")const FString& InNewName);

	// UFUNCTION(BlueprintImplementableEvent, Category = "GmDMChatSystem|Invalid Action", DisplayName = "When an invalid player name is set")
	// void Gb1_SetInvalidPlayerName(const FString& InInvalidPlayerName);

	DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_OneParam(FOnSetInvalidPlayerName, UVictimsChatManager, OnSetInvalidPlayerName, const FString&, InInvalidPlayerName);
	UPROPERTY(BlueprintAssignable, Category = "GmDMChatSystem|Invalid Action")
	FOnSetInvalidPlayerName OnSetInvalidPlayerName;
	
	UFUNCTION(BlueprintCallable, Category = "GmDMChatSystem")
	void ReceiveMsg(const FVictims_MsgInfo& InMsgInfo);

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "GmDMChatSystem")
	const FChatPartyInfo& GetPartyInfo();

	UFUNCTION(Server, Reliable, BlueprintCallable, DisplayName = "Create New Party", Category = "GmDMChatSystem")
	void Server_CreateNewParty();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "GmDMChatSystem")
	void ExpelPartyMember(AActor* InTargetMember);

	UFUNCTION(Server, Reliable, BlueprintCallable, DisplayName = "Invite New Party Member By Name", Category = "GmDMChatSystem")
	void Server_SendPartyInvitationsByName(const FString& InTargetPlayerName);

	UFUNCTION(Server, Reliable, BlueprintCallable, DisplayName = "Send Whisper Message", Category = "GmDMChatSystem")
	void Server_SendWhisperMsg(const FString& InTargetID, const FVictims_MsgInfo& InMsgInfo);

	/**
	 * 
	 * @param InNewPartyMember PlayerState of newly added party members
	 */
	UFUNCTION(Server, Reliable, BlueprintCallable, DisplayName = "Add New Party Member", Category = "GmDMChatSystem")
	void Server_AddNewPartyMember(AActor* InNewPartyMember);

	UFUNCTION(Server, Reliable, BlueprintCallable, DisplayName = "Break Party", Category = "GmDMChatSystem")
	void Server_BreakParty();

	UFUNCTION(Server, Reliable, BlueprintCallable, DisplayName = "Leave Party", Category = "GmDMChatSystem")
	void Server_LeaveParty();

	UFUNCTION(Server, Unreliable)
	void Server_ProcessMsg(const FVictims_MsgInfo& InMsgInfo);

	UFUNCTION(Server, Unreliable)
	void Server_ReceivedGameChat();

	UFUNCTION(Server, Unreliable)
	void Server_ReceivedSystemLogChat();

	UFUNCTION(Server, Unreliable)
	void Server_ReceivedPartyChat();

	UFUNCTION(Server, Unreliable)
	void Server_ReceivedPrivateChat();
	
	UPROPERTY()
	TObjectPtr<UVictims_ChatBox> ChatWidgetRef;

	UFUNCTION(Client, Unreliable)
	void Client_CreateInvitePartyWidget();

	UFUNCTION(Server, Unreliable)
	void Server_ApplyForPartyEntry();

	UFUNCTION(BlueprintCallable)
	void GmOnPressedWhisperKeyFunc();

	UPROPERTY(Replicated, BlueprintReadWrite)
	FChatPartyInfo PartyInfo;

	UFUNCTION(BlueprintCallable)
	UVictimsChatManager* GetChatManagerFromActor(AActor* InTarget) const;

protected:

	//~UActorComponent interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of UActorComponent interface


private:

	friend class UVictims_ChatSubsystem;
	
	UPROPERTY(Replicated)
	FString PlayerName;

	

	UPROPERTY()
	FVictims_MsgInfo MsgInfo;
	
	FString SimpleLeaderID;

	APlayerState* GetPSFromActor(AActor* InTarget) const;
	
	bool IsLeaderOfTheParty() const;

	// Bind input functions
	
	UFUNCTION()
	void GmOnPressedEnterFunc();
	
	UFUNCTION()
	void GmOnPressedSlashFunc();

	//UFUNCTION()
	//void GmOnPressedWhisperKeyFunc();

	UFUNCTION(Client, Unreliable)
	void Client_ReceivedChat(const FVictims_MsgInfo& InMsgInfo);
	
	UVictims_ChatSubsystem* GetChatSubsystem() const;
	TMap<TObjectPtr<AActor>, TObjectPtr<UVictimsChatManager>> GetRegisteredChatManagers() const;
	
};