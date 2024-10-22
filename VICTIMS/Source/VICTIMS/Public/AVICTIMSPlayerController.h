
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InventoryHUDInterface.h"
#include "Tuples.h"

#include "PartyComponent.h"
#include "AVICTIMSPlayerController.generated.h"

class UDatatable;
class AVICTIMSCharacter;
class UEquipmentComponent;
class UInventoryManagerComponent;
class UInteractiveText_Entry;
class UTestIDWidget;
class USeqPlayWidget;


DECLARE_DELEGATE_OneParam(FOnChangedPlayerList, TArray<FString>);

UCLASS()
class VICTIMS_API AVICTIMSPlayerController : public APlayerController, public IInventoryHUDInterface
{
	GENERATED_BODY()

public:

	AVICTIMSPlayerController();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AVICTIMSCharacter* CharacterReference = nullptr;

	// ========================================================================================================
	// AIAIÆaAI½º 
	// ========================================================================================================한음
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	APawn* MocapCharCP;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString playerName;

	// ========================================================================================================
	UFUNCTION(BlueprintCallable, Category = "Runtime Inspector")
	int GetCurrentViewMode(const APlayerController* PlayerController);

	virtual void OnPossess(APawn* aPawn) override;

	virtual void Tick(float DeltaTime) override;

	/* Interface */
	virtual void UI_MoveInventoryItem_Implementation(const uint8& FromInventorySlot, const uint8& ToInventorySlot) override;
	virtual void UI_DropInventoryItem_Implementation(const uint8& InventorySlot) override;
	virtual void UI_EquipInventoryItem_Implementation(const uint8& FromInventorySlot, const uint8& ToInventorySlot) override;
	virtual void UI_UnEquipInventoryItem_Implementation(const uint8& FromInventorySlot, const uint8& ToInventorySlot) override;
	virtual void UI_UseInventoryItem_Implementation(const uint8& InventorySlot) override;
	virtual void UI_UseContainerItem_Implementation(const uint8& InventorySlot) override;
	virtual void UI_UseHotbarItem_Implementation(const uint8& InventorySlot) override;
	virtual void UI_TakeContainerItem_Implementation(const uint8& FromContainerSlot, const uint8& ToInventorySlot) override;
	virtual void UI_DepositContainerItem_Implementation(const uint8& FromInventorySlot, const uint8& ToInventorySlot) override;
	virtual void UI_MoveContainerItem_Implementation(const uint8& FromInventorySlot, const uint8& ToInventorySlot) override;
	virtual void UI_EquipFromContainer_Implementation(const uint8& FromInventorySlot, const uint8& ToInventorySlot) override;
	virtual void UI_UnEquipToContainer_Implementation(const uint8& FromInventorySlot, const uint8& ToInventorySlot) override;
	virtual void UI_MoveHotbarItem_Implementation(const uint8& FromSlot, const uint8& ToSlot, const bool IsDraggedFromInventory, const bool IsDraggedFromHotbar) override;
	virtual void UI_PerChaseItem_Implementation(const uint8& InventorySlot) override;
	virtual void UI_SellItem_Implementation(const uint8& InventorySlot) override;
	/* Ends Interface */

	int32 UIGetPlayerGold();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (Category = "Inventory", OverrideNativeName = "InventoryComponent"))
	UInventoryManagerComponent* InventoryManagerComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (Category = "Inventory", OverrideNativeName = "EquipmentComponent"))
	UEquipmentComponent* PlayerInventoryComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class AMyHUD* HUD_Reference;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UHUDLayout* HUDLayoutReference;

	UFUNCTION()
	void SetupHUDReferences();

	UFUNCTION()
	void CollectFromPanel(const FName& Name);

	UFUNCTION()
	bool IsContainerOpen();
	UFUNCTION()
	bool IsShopOpen();

	UFUNCTION(BlueprintCallable, Category = "Character")
	void ToggleInventory();
	UFUNCTION(BlueprintCallable, Category = "Character")
	void ToggleProfile();
	void SetInputDependingFromVisibleWidgets();
	UFUNCTION(BlueprintCallable, Category = "Character")
	void ToggleContainer();
	UFUNCTION(BlueprintCallable, Category = "Character")
	void ToggleShop();

	UFUNCTION(BlueprintCallable, Category = "Character")				// 사용 안하는 중. 
		void ToggleMenu();

	UFUNCTION(Server, Reliable)
	void Server_OnActorUsed(AActor* Actor);

	UFUNCTION()
	UUserWidget* GenerateInteractWidget(FText Text);
	UFUNCTION()
	UUserWidget* CreateInteractWidget(FName Name);
	UFUNCTION()
	void SetMouseToCenterPosition();

	UFUNCTION(BlueprintCallable, Category = "Character")
	void EnableUIMode();
	UFUNCTION(BlueprintCallable, Category = "Character")
	void DisableUIMode();

	UFUNCTION()
	UDataTable* GetItemDB();

	uint8 GetMaximumHotbarSlots() { return MaximumHotbarSlots; };

	uint8 MenuAnchorIndex;

	FReturnTupleSlotNativeID MenuAnchorSlotIndex;
	UFUNCTION()
	void UseHotbarSlot1();
	UFUNCTION()
	void UseHotbarSlot2();
	UFUNCTION()
	void UseHotbarSlot3();
	UFUNCTION()
	void UseHotbarSlot4();
	UFUNCTION()
	void UseHotbarSlot5();

	UFUNCTION(BlueprintCallable, Category = "Character")
	void Interact();

	UFUNCTION()
	void OnActorUsed(AActor* Actor1);

	UFUNCTION(BlueprintCallable)
	void RequestClientTravel(const FString& URL, const FString& Options);

	UFUNCTION(Server, Reliable)
	void ServerRPC_RequestClientTravel(const FString& URL, const FString& Options);

	UFUNCTION(Client, Reliable)
	void ClientRPC_RequestClientTravel(const FString& URL, const FString& Options);

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite)
	bool bUseUIMode = false;

	UFUNCTION()
	void SavePersonalID(FString ID);

	UFUNCTION(Server, Reliable)
	void ServerRPC_SavePersonalID(const FString& ID);

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticastRPC_SavePersonalID(const FString& ID);

	UPROPERTY()
	bool bInventoryInitialized = false;

	UPROPERTY(BlueprintReadOnly)
	bool bHasBullet = false;

protected:
	virtual void BeginPlay() override;

private:
	uint8 MaximumHotbarSlots = 10;


	//=========================================================================================================================
	// Save

public:

	UFUNCTION()
	void HandleEscapeKey();

	UPROPERTY()
	FString PlayerID;

	//UPROPERTY()
	//class UTestSaveGame* SavedData;

	UPROPERTY(Replicated)
	bool bNeedToLoad = false;

	UFUNCTION()
	void CreateSaveData(FString ID);		// 새로 저장파일 만들기

	UFUNCTION()
	UTestSaveGame* GetSaveDataFromID(FString ID);		// TestIDWidget 에서 입력받은 문자열 ID 와 맞는 TesTSaveGame 데이터 가져오기

	UFUNCTION()
	void SaveData();						// 데이터 저장 

	UFUNCTION(Server, Reliable)
	void ServerRPC_SaveData();		// 데이터 저장 

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticastRPC_SaveData();		// 데이터 저장 

	UFUNCTION()
	void LoadData(FString ID);

	UFUNCTION(Server, Reliable)
	void ServerRPC_LoadData(const FString& ID);		// 데이터 저장 

	UFUNCTION(Client, Reliable)
	void NetMulticastRPC_LoadData(bool bSuccess);		// 데이터 저장 

	UFUNCTION(Server, Reliable)
	void ServerRPC_RestoreBuildComps();

	UFUNCTION(Server, Reliable)
	void ServerRPC_SearchBuildComps();

	UFUNCTION(Client, Reliable)
	void ClientRPC_SetBuildCompsData(const TArray<struct FBuildSaveDataStruct>& _buildCompsData);

	UPROPERTY()
	TArray<struct FBuildSaveDataStruct> buildCompsData;

	UPROPERTY()
	UTestIDWidget* TestIDWidget;

	UPROPERTY(EditAnywhere, Category = "Test")
	TSubclassOf<UTestIDWidget> TestIDWidget_bp;

	UPROPERTY()
	class UIDInValidWidget* IDInValidWidget;

	UPROPERTY(EditAnywhere, Category = "Test")
	TSubclassOf<class UIDInValidWidget> IDInvalidWidget_bp;


	UFUNCTION(Server, Reliable)
	void ServerRPC_SetUseUIState(bool bUse);

	UFUNCTION(Client, Reliable)
	void ClientRPC_SetUseUIState(bool bUse);

	UFUNCTION(Client, Reliable)
	void ClientRPC_SaveHotbar(const FString& ID);

	UFUNCTION(Server, Reliable)
	void ServerRPC_SendHotbarData(const FString& ID, const FString& HotbarItems);

	UFUNCTION(Client, Reliable)
	void ClientRPC_LoadHotbar(const FString& ID, const FSlotStructure& ComparedItem, const TArray<FString>& savedHotbarItemNames);

	UFUNCTION(Server, Reliable)
	void ServerRPC_LoadHotbarData(const FString& ID, const uint8& Index, const FSlotStructure& HotbarItems);

	UFUNCTION()
	void CloseTestIDWidget();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	class AVICTIMSGameMode* GameModeReference;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test")
	bool bIsShowUI = false;

	FTimerHandle HousingNotificationTimerHandle;
	void ShowHousingNotification();
	void HideHousingNotification();

	UFUNCTION()
	void EnableHousingTutorialWidget(bool bEnable);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerRPC_TryReload(FName Bullet);

	UPROPERTY()
	class UHousingNotification* HousingNotificationWidget;

	UPROPERTY(EditAnywhere, Category = "Test")
	TSubclassOf<class UHousingNotification> HousingNotificationWidgetClass;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void ShowMovingInfo();

	UPROPERTY(EditDefaultsOnly, Category = "Test")
	TSubclassOf<class UMovingInfoWidget> MovingInfoWidgetClass;

	UPROPERTY()
	UMovingInfoWidget* MovingInfoWidget;
	//=========================================================================================================================

	UFUNCTION()
	void CloseLayouts();

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UTabMenuWidget> TabMenu_wbp;

	UPROPERTY()
	UTabMenuWidget* TabMenu;

	UPROPERTY(EditAnywhere, Category = "Interact Sound")
	class USoundBase* PickUpSound;

	UPROPERTY(EditAnywhere, Category = "Interact Sound")
	class USoundBase* ToggleSound;

	UPROPERTY()
	TArray<AVICTIMSPlayerController*> otherPlayers;

	UPROPERTY()
	TArray<FString> otherPlayerNames;

	UFUNCTION(Server, Reliable)
	void ServerRPC_UpdatePlayerList();

	UFUNCTION(Client, Reliable)
	void ClientRPC_UpdatePlayerList(const TArray<AVICTIMSPlayerController*>& otherPlayerList);

	UFUNCTION(Client, Reliable)
	void ClientRPC_UpdatePlayerNameList(const TArray<FString>& otherPlayerNameList);

	FOnChangedPlayerList OnChangedPlayerList;

	UPROPERTY(EditAnywhere, Category = "Start Menu Sound")
	class USoundBase* BGM;

	UPROPERTY(EditAnywhere, Category = "Start Menu Sound")
	class UAudioComponent* BGMComp;

	UPROPERTY(BlueprintReadWrite)
	bool bWidgetBlocked;

	// Loading
	UPROPERTY(EditAnyWhere)
	TSubclassOf<class ULoadingWidget> bp_LoadingWidget;

	UPROPERTY()
	class ULoadingWidget* LoadingWidget;

	UFUNCTION(Client, Reliable)
	void ClientRPC_ShowLoadingUI();

	UFUNCTION(Client, Reliable)
	void ClientRPC_AddTag(const FName& newTag);

	UFUNCTION()
	void HideWidgets();

	UFUNCTION(BlueprintImplementableEvent)
	void ChatManagerEnable(const FString& chatPlayerName);

	UFUNCTION(Client, Reliable)
	void ClientRPC_EnableHousingTipText(bool bEnable, bool bItem, bool bHousing, bool bHousingDel);

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UHousingTutorialWidget> TutorialWidget_wbp;

	UPROPERTY(EditAnywhere, Category = "SEQ Sound")
	class UAudioComponent* SeqSoundComp;

	///////////////////// SEQ /////////////////////
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	TSubclassOf<USeqPlayWidget> bp_SeqplayWidget;

	UFUNCTION(BlueprintCallable)
	void VicTimsSeqPlay();

	UFUNCTION(Server, Reliable)
	void ServerRPC_RequsetSeqPlay();

	UFUNCTION(Client, Reliable)
	void ClientRPC_RequsetSeqPlay();

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	bool bSeqIsPlaying = false;

	UFUNCTION(BlueprintCallable)
	void SetSeqPlayingState(bool bOn);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerRPC_SetSeqPlayingState(bool bOn);

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void ClientRPC_SetSeqPlayingState(bool bOn);
};
