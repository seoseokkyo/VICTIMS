
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InventoryHUDInterface.h"
#include "Tuples.h"
#include "AVICTIMSPlayerController.generated.h"

class UDatatable;
class AVICTIMSCharacter;
class UEquipmentComponent;
class UInventoryManagerComponent;
class UInteractiveText_Entry;
class UTestIDWidget;

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
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(Category="Inventory", OverrideNativeName="InventoryComponent"))
	UInventoryManagerComponent* InventoryManagerComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(Category="Inventory", OverrideNativeName="EquipmentComponent"))
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
	
	UFUNCTION(BlueprintCallable, Category="Character")
	void ToggleInventory();
	UFUNCTION(BlueprintCallable, Category="Character")
	void ToggleProfile();
	void SetInputDependingFromVisibleWidgets();
	UFUNCTION(BlueprintCallable, Category="Character")
	void ToggleContainer();
	UFUNCTION(BlueprintCallable, Category="Character")
	void ToggleShop();
	
	UFUNCTION(BlueprintCallable, Category="Character")				// 사용 안하는 중. 
	void ToggleMenu();

	UFUNCTION(Server, Reliable)
	void Server_OnActorUsed(AActor* Actor);
	
	UFUNCTION()
	UUserWidget* GenerateInteractWidget(FText Text);
	UFUNCTION()
	UUserWidget* CreateInteractWidget(FName Name);
	UFUNCTION()
	void SetMouseToCenterPosition();

	UFUNCTION(BlueprintCallable, Category="Character")
	void EnableUIMode();
	UFUNCTION(BlueprintCallable, Category="Character")
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

protected:
	virtual void BeginPlay() override;
	
private:
	uint8 MaximumHotbarSlots = 10;


//=========================================================================================================================
// Save

public:


	UPROPERTY()
	FString PlayerID;						

	//UPROPERTY()
	//class UTestSaveGame* SavedData;

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

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticastRPC_LoadData(bool bSuccess);		// 데이터 저장 

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

	UFUNCTION()
	void CloseTestIDWidget();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	class AVICTIMSGameMode* GameModeReference;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Test")
	bool bIsShowUI = false;

	FTimerHandle HousingNotificationTimerHandle;
	void ShowHousingNotification();
	void HideHousingNotification();

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

	void PopulatePlayerList();

	UFUNCTION(Server, Reliable)
	void Server_RequestPlayerList();

	UFUNCTION(Client, Reliable)
	void Client_ReceivePlayerList(const TArray<FString>& PlayerNames);
	
	TArray<FString> AddedPlayerNames;

	virtual void OnRep_PlayerState() override;
	void UpdateAllClientsPlayerList();

	UFUNCTION(Server, Reliable)
	void Server_RequestPlayerListUpdate();
//=========================================================================================================================

	UFUNCTION()
	void CloseLayouts();

    UPROPERTY(EditAnywhere, Category = "Interact Sound")
	class USoundBase* PickUpSound;

};
