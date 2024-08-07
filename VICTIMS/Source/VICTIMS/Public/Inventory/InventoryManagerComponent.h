
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EEquipmentSlot.h"
#include "EItemType.h"
#include "FContainerInfo.h"
#include "FSlotStructure.h"
#include "FShopInfo.h"
#include "FShopList.h"
#include "InventoryManagerComponent.generated.h"

class UDataTable;
class UMainLayout;
class UInventoryComponent;
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class VICTIMS_API UInventoryManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInventoryManagerComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_UpdateGoldAmount();

	UPROPERTY(Replicated, ReplicatedUsing = OnRep_UpdateGoldAmount)
	int32 Gold;

	UFUNCTION()
	void InitializeItemDB();

	UPROPERTY()
	class AVICTIMSCharacter* playerReference;

	UPROPERTY()
	class AMyHUD* HUD;

	UPROPERTY()
	class UHousingComponent* housingComponent;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:

	UFUNCTION(BlueprintCallable, Category = "Manager|Public|Items")
	void UseBulletItem(FName Bullet);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Manager|Public|Items")
	void ServerRPC_UseBulletItem(FName Bullet, const int32 count, uint8 Slot);

	UFUNCTION(Client, Reliable, BlueprintCallable, Category = "Manager|Public|Items")
	void ClientRPC_UseBulletItem(const int32 maxBullets, const int32 count);

	UPROPERTY()
	int32 MaxBullet;

	UPROPERTY()
	int32 CurrentBullet;

	UPROPERTY(BlueprintReadWrite)
	bool bBulletSuccess;

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void ClientRPC_BulletNotification();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void InitializeInventoryManager(UInventoryComponent* EquipmentComponent);

	UFUNCTION(Server, Reliable)
	virtual void Server_InitInventory();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (DisplayName = "Player Inventory", Category = "Default", OverrideNativeName = "PlayerInventory"))
	UInventoryComponent* PlayerInventory;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (DisplayName = "Container Inventory", Category = "Default", OverrideNativeName = "ContainerInventory"))
	UInventoryComponent* ContainerInventory;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	uint8 TotalNumberOfSlots;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UserInterface")
	UMainLayout* MainLayoutUI;

	UFUNCTION(Category = "Manager|Public")
	void TryToAddItemToInventory(UInventoryComponent* Inventory, FSlotStructure& InventoryItem, bool& bOutSuccess);

	UFUNCTION(Server, Reliable)
	void Server_UseInventoryItem(const uint8& InventorySlot);

	UFUNCTION(Server, Reliable)
	void Server_UseHotbarWeapon(const uint8& InventorySlot);

	UFUNCTION(Client, Reliable)
	void Client_UseHotbarWeapon(const uint8& HotbarSlot);

 	UFUNCTION(Server,Reliable)
 	void Server_CheckHotbarWeapon(const FSlotStructure& DroppingItem);

	UFUNCTION(Client, Reliable)
	void Client_CheckHotbarWeapon(const FSlotStructure& Slots);
 
	UFUNCTION(Server, Reliable)
	void Server_CheckPistolBullet();

	UFUNCTION(Client, Reliable)
	void Client_CheckPistolBullet(const uint8& NewPistolBullet);

	UFUNCTION(Server, Reliable)
	void Server_CheckShotgunBullet();

	UFUNCTION(Client, Reliable)
	void Client_CheckShotgunBullet(const uint8& NewShotGunBullet);

	UFUNCTION(Server, Reliable)
	void Server_UseContainerItem(const uint8& InventorySlot);

	UFUNCTION(Server, Reliable)
	void Server_MoveInventoryItem(uint8 FromInventorySlot, uint8 ToInventorySlot);

	UFUNCTION(Server, Reliable)
	void Server_EquipFromInventory(uint8 FromInventorySlot, uint8 ToInventorySlot);

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_EquipFromInventory(uint8 FromInventorySlot, uint8 ToInventorySlot);

	UFUNCTION(Server, Reliable)
	void Server_UnEquipToInventory(uint8 FromInventorySlot, uint8 ToInventorySlot);

	UFUNCTION(Server, Reliable)
	void Server_DropItemFromInventory(const uint8& InventorySlot);

	UFUNCTION(Server, Reliable)
	void Server_UseContainer(AActor* Container);
	UFUNCTION(Server, Reliable)
	void Server_CloseContainer();

	UFUNCTION(Server, Reliable)
	void Server_TakeContainerItem(const uint8& FromContainerSlot, const uint8& ToInventorySlot);
	UFUNCTION(Server, Reliable)
	void Server_DepositContainerItem(const uint8& FromInventorySlot, const uint8& ToInventorySlot);
	UFUNCTION(Server, Reliable)
	void Server_MoveContainerItem(uint8 FromInventorySlot, uint8 ToInventorySlot);

	UFUNCTION(Server, Reliable)
	void Server_EquipFromContainer(uint8 FromInventorySlot, uint8 ToInventorySlot);
	UFUNCTION(Server, Reliable)
	void Server_UnEquipToContainer(uint8 FromInventorySlot, uint8 ToInventorySlot);

	UFUNCTION(Client, Reliable)
	void Client_SetInventorySlotItem(const FSlotStructure& ContentToAdd, const uint8& InventorySlot);
	UFUNCTION(Client, Reliable)
	void Client_SetContainerSlotItem(const FSlotStructure& ContentToAdd, const uint8& InventorySlot);

	UFUNCTION(Client, Reliable)
	void Client_ClearInventorySlotItem(uint8 InventorySlot);
	UFUNCTION(Client, Reliable)
	void Client_ClearContainerSlotItem(uint8 ContainerSlot);

	UFUNCTION(Client, Reliable)
	void Client_OpenContainer(FContainerInfo ContainerProperties, const TArray<FSlotStructure>& InContainerInventory, const TArray<FSlotStructure>& InPlayerInventory);
	UFUNCTION(Client, Reliable)
	void Client_CloseContainer();

	UFUNCTION(Client, Reliable)
	void Client_MoveHotbarSlotItem(const uint8& FromSlot, const uint8& ToSlot, const bool IsDraggedFromInventory, const bool IsDraggedFromHotbar);

	UFUNCTION(Client, Reliable)
	void Client_MoveHotbarSlotItem1(const uint8& FromSlot, const uint8& ToSlot, const int& Index,const FSlotStructure& Slot, const bool bEquipped);

	UFUNCTION(Client, Reliable)
	void Client_MoveHotbarSlotItem2(const uint8& FromSlot, const uint8& ToSlot, const FSlotStructure& FromItem, const FSlotStructure& ToItem);

	UFUNCTION(Server, Reliable)
	void Server_MoveHotbarSlotItem(const uint8& FromSlot, const uint8& ToSlot, const TArray<FSlotStructure>& HotbarInventory, const FSlotStructure& InPlayerInventory, const FSlotStructure& InEquipmentInventory);
	
	UFUNCTION(Server, Reliable)
	void Server_UseHotbarSlot(const uint8& HotbarSlot, const FSlotStructure& HotbarItem);

	UFUNCTION(Client, Reliable)
	void Client_UseHotbarSlot(const uint8& HotbarSlot);

	UFUNCTION(Client, Reliable)
	void Client_UseHotbarItem(const uint8& HotbarSlot, const FSlotStructure& Slot, const FSlotStructure& Slot2, const bool bEquipping);

	UFUNCTION(Client, Reliable)
	void Client_ClearHotbarSlot(const uint8& HotbarSlot);

	UFUNCTION(Client, Reliable)
	void Client_ClearHotbarWeapon(const FSlotStructure& HotbarSlot);

	UFUNCTION(Server, Reliable)
	void Server_UpdateTooltips();

	UFUNCTION(Client, Reliable)
	void Client_LoadInventoryUI();
	UFUNCTION(Client, Reliable)
	void Client_LoadProfileUI();
	UFUNCTION(Client, Reliable)
	void Client_LoadHotbarUI();

	UFUNCTION(Client, Reliable)
	void Client_CheckHotbarSlots(const FSlotStructure& Slot);

	UFUNCTION()
	void LoadHotbarUI();

	UFUNCTION(Category = "Manager|Public")
	void InitializeInventoryManagerUI(UMainLayout* MainLayout);

	UFUNCTION(Category = "Manager|Private")
	EEquipmentSlot GetEquipmentTypeBySlot(const uint8& EquipmentSlot);

	UFUNCTION()
	EItemType GetItemTypeBySlot(uint8 ItemSlot);

	UFUNCTION(Category = "Manager|Private|Inventory")
	void AddItem(UInventoryComponent* Inventory, uint8 InventorySlot, FSlotStructure& InventoryItem);

	UFUNCTION()
	FSlotStructure GetEmptySlot(EEquipmentSlot FromEquipmentType);

	FSlotStructure GetItemFromItemDB(FName Name);

	UFUNCTION(Client, Reliable)
	void Client_SetAttributes(int32 Value);

	UFUNCTION()
	void UpdateEquippedStats();

	UFUNCTION(Category = "Manager|Getter")
	UDataTable* GetItemDB();

	UFUNCTION()
	void InitializePlayerAttributes();

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	TArray<uint8> AttributesArray;

	UPROPERTY()
	AActor* CurrentContainer;


	/*Defined on Player Controller*/
	uint8 NumberOfRowsInventory = 0;
	uint8 SlotsPerRowInventory = 0;
	uint8 NumberOfSlotsOnHotbar = 0;

	UFUNCTION(Category = "Manager|Private|Inventory")
	void DropItem(UInventoryComponent* Inventory, uint8 InventorySlot);

	UFUNCTION(Client, Reliable)
	void Client_DropBullet(const FSlotStructure& DroppedItem);

	
	UFUNCTION(Category = "Manager|Private|Inventory")
	void AddGold(int32 Amount);

	UFUNCTION(Category = "Manager|Private|Equipment")
	void EquipItem(UInventoryComponent* FromInventory, uint8 FromInventorySlot,	UInventoryComponent* ToInventory, uint8 ToInventorySlot);

	UFUNCTION(Category = "Manager|Private|Items")
	void UseInventoryItem(const uint8& InventorySlot);

	UFUNCTION(Category = "Manager|Private")
	void RandomizeDropLocation(FSlotStructure LocalSlot, UClass*& LocalClass, FTransform& OutTransform);
	
	UFUNCTION(Category = "UserInterface|Private|Hotbar")
	void SetHotbarSlotItem(const uint8& ToSlot, FSlotStructure SlotStructure);

	UFUNCTION(Client, Reliable)
	void Client_SetHotbarSlotItem(const uint8& ToSlot, FSlotStructure SlotStructure);

	UFUNCTION(Category = "UserInterface|Private|Hotbar")
	FSlotStructure GetHotbarSlotItem(const uint8& HotbarSlot);

protected:
	UFUNCTION(Client, Reliable)
	void Client_UpdateInventoryTooltips(const TArray<FSlotStructure>& InPlayerInventory, const TArray<FSlotStructure>& InOtherInventory);
	UFUNCTION(Client, Reliable)
	void Client_UpdateContainerTooltips(const TArray<FSlotStructure>& InPlayerInventory, const TArray<FSlotStructure>& InOtherInventory);

private:
	UFUNCTION(Category = "UserInterface|Private|Hotbar")
	void MoveHotbarSlotItem(const uint8& FromSlot, const uint8& ToSlot, const bool IsDraggedFromInventory, const bool IsDraggedFromHotbar);


	UFUNCTION(Server, Reliable)
	void ServerRPC_Test(const uint8& FromSlot, const uint8& ToSlot, const TArray<FSlotStructure>& arrayTemp, const FSlotStructure& InEquipmentInventory);

	UFUNCTION(Category = "UserInterface|Private|Hotbar")
	void UseHotbarSlot(const uint8& HotbarSlot, const FSlotStructure& Slot, const FSlotStructure& Slot2, const bool bEquipping);
	UFUNCTION(Category = "UserInterface|Private|Hotbar")
	void ClearHotbarSlotItem(const uint8& HotbarSlot);


	UPROPERTY()
	UDataTable* ItemDB;

	UFUNCTION(Category = "Helper")
	uint8 GetEquipmentSlotByType(EEquipmentSlot EquipmentSlot);
	UFUNCTION(Category = "Helper")
	EEquipmentSlot GetItemEquipmentSlot(FSlotStructure Item);
	UFUNCTION(Category = "Helper")
	bool ItemIsValid(FSlotStructure Item);
	UFUNCTION(Category = "Helper")
	bool GetEmptyEquipmentSlotByType(EEquipmentSlot EquipmentSlot, uint8& OutIndex);
	UFUNCTION(Category = "Helper")
	bool ItemIsSame(const FSlotStructure Item1, const FSlotStructure Item2);
	UFUNCTION(Category = "Helper")
	bool IsItemStackable(const FSlotStructure Item);
	UFUNCTION(Category = "Helper")
	uint8 ItemFreeStackSpace(const FSlotStructure Item);
	UFUNCTION(Category = "Helper")
	uint8 GetItemAmount(const FSlotStructure Item);
	UFUNCTION(Category = "Helper")
	uint8 GetItemMaxStackSize(const FSlotStructure Item);
	UFUNCTION(Category = "Helper")
	void AddAmountToItem(FSlotStructure& Item, uint8 AmountToAdd); 

	UFUNCTION()
	void SetAttributes(int32 Value);

	UFUNCTION()
	void RemoveFromItemAmount(FSlotStructure& InventoryItem, const uint8& AmountToRemove, bool& WasFullAmountRemoved, uint8& AmountRemoved);

	UFUNCTION(Category = "Manager|Private|Equipment")
	void UnEquipItem(UInventoryComponent* FromInventory, uint8 FromInventorySlot,UInventoryComponent* ToInventory, uint8 ToInventorySlot);

	UFUNCTION(Category = "Manager|Private|Inventory")
	void MoveItem(UInventoryComponent* FromInventory, uint8 FromInventorySlot, UInventoryComponent* ToInventory, uint8 ToInventorySlot);
	UFUNCTION(Category = "Manager|Private|Inventory")
	void RemoveItem(UInventoryComponent* Inventory, uint8 InventorySlot);

	UFUNCTION(Category = "Manager|Private|Items")
	void UseEquipmentItem(uint8 InventorySlot, FSlotStructure InventoryItem, UInventoryComponent* ToInventory);

	UFUNCTION(Category = "Manager|Private|Items")
	void UseConsumableItem(uint8 InventorySlot, FSlotStructure InventoryItem);

	UFUNCTION(Client, Reliable, Category = "Manager|Private|Items")
	void ClientRPC_UseConsumableItem(const int32 Health);


	UFUNCTION(Category = "Manager|Private|Items")
	void UseFurnitureItem(uint8 InventorySlot, FSlotStructure InventoryItem);

	UFUNCTION(Client, Reliable, Category = "Manager|Private|Items")
	void ClientRPC_UseFurnitureItem(FName ItemID);


	UFUNCTION(Category = "Manager|Private|Container")
	void UseContainerItem(const uint8& InventorySlot);

	UFUNCTION(Category = "UserInterface|Private|Inventory")
	void ClearInventoryItem(const uint8& InventorySlot);
	UFUNCTION(Category = "UserInterface|Private|Inventory")
	void SetInventorySlotItem(const FSlotStructure& ContentToAdd, const uint8& InventorySlot);
	UFUNCTION(Category = "UserInterface|Private|Inventory")
	void ClearInventorySlotItem(uint8 InventorySlot);

	UFUNCTION(Category = "UserInterface|Private|Inventory")
	FSlotStructure GetInventorySlotItem(uint8 InventorySlot);

	UFUNCTION(Category = "UserInterface|Private|Container")
	void ClearContainerSlots();
	UFUNCTION(Category = "UserInterface|Private|Container")
	void CreateContainerSlots(uint8 NumberOfRows, uint8 SlotsPerRow);
	UFUNCTION(Category = "UserInterface|Private|Container")
	void SetViewersContainerSlot(uint8 ContainerSlot, FSlotStructure& InventoryItem);

	UFUNCTION(Category = "UserInterface|Private|Container")
	void CreateContainerSlots2(uint8 InventorySize, uint8 SlotsPerRow);

	UFUNCTION(Category = "UserInterface|Private|Container")
	void AddContainerSlot(uint8 Row, uint8 Column, uint8 Slot, bool IsStorage);


	UFUNCTION(Category = "UserInterface|Private|Container")
	void ClearContainerSlotItem(uint8 ContainerSlot);
	UFUNCTION(Category = "UserInterface|Private|Container")
	void ClearViewersContainerSlot(uint8 ContainerSlot);
	UFUNCTION(Category = "UserInterface|Private|Container")
	void SetContainerSlotItem(const FSlotStructure& Slot, uint8 Index);

	UFUNCTION(Category = "Manager|Private|Container")
	void OpenContainer(AActor* Container);
	UFUNCTION(Category = "Manager|Private|Container")
	void UseContainer(AActor* Container);
	UFUNCTION(Category = "Manager|Private|Container")
	void CloseContainer();
	UFUNCTION(Category = "Manager|Private|Container")
	void LoadContainerSlots(FContainerInfo ContainerProperties, const TArray<FSlotStructure>& InContainerInventory, const TArray<FSlotStructure>& InPlayerInventory);

	UFUNCTION(Category = "Manager|Private")
	bool CanContainerStoreItems(UInventoryComponent* Inventory);

	UFUNCTION(Category = "Manager|Private|Stacks")
	void FindAndAddAmountToStacks(UInventoryComponent* Inventory, FName ItemID, uint8 Amount, uint8& AmountRemaining);

	UFUNCTION(Category = "Manager|Private|Stacks")
	void AddItemToStack(UInventoryComponent* Inventory, uint8 InventorySlot, uint8 AmountToAdd, uint8& AmountRemaining);

	UFUNCTION()
	void SetPlayerRef(class AVICTIMSCharacter* _playerReference) { playerReference = _playerReference; };

	UFUNCTION()
	class AVICTIMSCharacter* GetPlayerRef();

	UFUNCTION()
	void SetPlayerHud(class AMyHUD* _playerHud) { HUD = _playerHud; };

	UFUNCTION()
	class AMyHUD* GetPlayerHud();

//===========================================================================================================================================
//SHOP
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (DisplayName = "Shop Inventory", Category = "Default", OverrideNativeName = "Shop Inventory"))
	UInventoryComponent* ShopInventory;

	UPROPERTY()
	AActor* CurrentShop;

	UPROPERTY()
	bool IsSelling = false;

	UFUNCTION(Server,Reliable)
	void Server_UseShop(AActor* Shop); 
	UFUNCTION(Server, Reliable)
	void Server_CloseShop();		  

	UFUNCTION(Server,Reliable)
	void Server_PerchaseShopItem(const uint8& InventorySlot);
	UFUNCTION(Server,Reliable)
	void Server_SellItem(const uint8& InventorySlot); 
	UFUNCTION(Client, Reliable)
	void Client_SetShopSlotItem(const FSlotStructure& ContentToAdd, const uint8& InventorySlot);
	UFUNCTION(Client, Reliable)
	void Client_ClearShopSlotItem(uint8 ShopSlot);
	UFUNCTION(Client, Reliable)
	void Client_OpenShop(FShopInfo ShopProperties, const TArray<FSlotStructure>& InShopInventory, const TArray<FSlotStructure>& InPlayerInventory);
	UFUNCTION(Client, Reliable)
	void Client_CloseShop();
	UFUNCTION(Client, Reliable)
	void Client_UpdateShopTooltips(const TArray<FSlotStructure>& InPlayerInventory, const TArray<FSlotStructure>& InOtherInventory);

//======================================================================================================================================
// UI Sound

	UPROPERTY(EditAnywhere, Category = "UI Sound")
	class USoundBase* DropSound;

	UPROPERTY(EditAnywhere, Category = "UI Sound")
	class USoundBase* UseSound_Equipment;

	UPROPERTY(EditAnywhere, Category = "UI Sound")
	class USoundBase* UseSound_Consumable;

	UPROPERTY(EditAnywhere, Category = "UI Sound")
	class USoundBase* UseSound_Furniture;

	UPROPERTY(EditAnywhere, Category = "UI Sound")
	class USoundBase* MoveItemSound;

	UPROPERTY(EditAnywhere, Category = "UI Sound")
	class USoundBase* OpenShopSound;

	//<< SetWeaponIcon ฐüทร
	UFUNCTION(Client, Reliable)
	void ClientRPC_SetWeaponIcon(FName WeaponName, int32 CurrentBullets, int32 MaxBullets);

	UFUNCTION(Client, Reliable)
	void ClientRPC_HideWeaponIcon();

protected:

private:
	
	UFUNCTION(Category = "Manager|Private|Shop")
	void PerchaseShopItem(const uint8& InventorySlot);
	UFUNCTION(Category = "Manager|Private|Shop")
	void SellItem(const uint8& InventorySlot);
	
	UFUNCTION(Category = "UserInterface|Private|Shop")
	void ClearShopSlots();

	UFUNCTION(Category = "UserInterface|Private|Shop")
	void CreateShopSlots(uint8 NumberOfRows, uint8 SlotsPerRow);
	UFUNCTION(Category = "UserInterface|Private|Shop")
	void CreateShopSlots2(uint8 InventorySIze, uint8 SlotsPerRow);

	UFUNCTION(Category = "UserInterface|Private|Shop")
	void SetViewersShopSlot(uint8 ShopSLot, FSlotStructure& InventoryItem);
	UFUNCTION(Category = "UserInterface|Private|Shop")
	void AddShopSlot(uint8 Row, uint8 Column, uint8 Slot, bool IsShop);

	UFUNCTION(Category = "UserInterface|Private|Shop")
	void ClearShopSlotItem(uint8 ShopSlot);
	UFUNCTION(Category = "UserInterface|Private|Shop")
	void ClearViewersShopSlot(uint8 ShopSlot);
	UFUNCTION(Category = "UserInterface|Private|Shop")
	void SetShopSlotItem(const FSlotStructure& Slot, uint8 Index);
	
	UFUNCTION(Category = "UserInterface|Private|Shop")
	void OpenShop(AActor* Shop);
	UFUNCTION(Category = "UserInterface|Private|Shop")
	void UseShop(AActor* Shop);
	UFUNCTION(Category = "UserInterface|Private|Shop")
	void CloseShop();
	
	UFUNCTION(Category = "UserInterface|Private|Shop")
	void LoadShopSlots(FShopInfo ShopProperties, const TArray<FSlotStructure>& InShopInventroy, const TArray<FSlotStructure>& InPlayerInventory);
	UFUNCTION(Category = "Manager|Private")
	bool CanShopItems(UInventoryComponent* Inventory);

public:
	
	UFUNCTION(Client, Reliable, Category = "USerInterface|public|DropMoney")
	void Client_ShowDropMoneyLayout();

	UFUNCTION(Client, Reliable, Category = "USerInterface|public|DropMoney")
	void Client_CloseDropMoneyLayout();

	UFUNCTION(Category = "UserInterface|Public|DropMoney")
	void DropMoney();

	void EquipItemAtLoad(UInventoryComponent* FromInventory, uint8 FromInventorySlot, UInventoryComponent* ToInventory, uint8 ToInventorySlot);

	UPROPERTY()
	uint8 DropMoneyAmount;

	UFUNCTION(Client, Reliable)
	void Client_ShowNotification(const uint8& InventorySlot);

//=======================================================================================================
// Hotbar weapon

	UPROPERTY(ReplicatedUsing = "OnRep_EquipPistol")
	bool bEquipPistol;

	UFUNCTION()
	void OnRep_EquipPistol(bool Equiped);

	UPROPERTY(ReplicatedUsing = "OnRep_EquipShotGun")
	bool bEquipShotGun;

	UFUNCTION()
	void OnRep_EquipShotGun(bool Equiped);


	UPROPERTY(ReplicatedUsing = "OnRep_EquipKnife")
	bool bEquipKnife;

	UFUNCTION()
	void OnRep_EquipKnife(bool Equiped);

	UPROPERTY(ReplicatedUsing = "OnRep_EquipAxe")
	bool bEquipAxe;

	UFUNCTION()
	void OnRep_EquipAxe(bool Equiped);

	UPROPERTY(Replicated)
	bool bEquipRifle;
	
	UFUNCTION()
	void OnChangeCloath(EEquipmentSlot equipslot);
};
