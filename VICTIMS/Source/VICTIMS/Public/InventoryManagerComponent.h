
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EEquipmentSlot.h"
#include "ItemDataStructs.h"
#include "FContainerInfo.h"
#include "FSlotStructure.h"
#include "InventoryManagerComponent.generated.h"

class UDataTable;
class UMainMenu;
class UInventoryComponent;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VICTIMS_API UInventoryManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventoryManagerComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_UpdateGoldAmount();

	UPROPERTY(Replicated, ReplicatedUsing = OnRep_UpdateGoldAmount)					// 돈 
	uint8 Gold;

	UFUNCTION()
	void InitializeItemDB();

protected:
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (DisplayName = "Player Inventory", Category = "Default", OverrideNativeName = "PlayerInventory"))
	UInventoryComponent* playerInventory;		// 플레이어 인벤토리

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (DisplayName = "Container Inventory", Category = "Default", OverrideNativeName = "ContainerInventory"))
	UInventoryComponent* containerInventory;	// 보관함 

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	uint8 totalNumberOfSlots;					// 슬롯 개수

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "UserInterface")
	UMainMenu* mainLayoutUI; 

	UFUNCTION(Category = "Manager|Public")			
	void TryToAddItemToInventory(UInventoryComponent* Inventory, FSlotStructure& InventoryItem, bool& bOutSuccess);	// 아이템 추가

	UFUNCTION(Server, Reliable)																						// 서버_인벤토리 아이템 사용
	void Server_UseInventoryItem(const uint8& InventorySlot);
				
	UFUNCTION(Server, Reliable)																						// 서버_보관함 아이템 사용
	void Server_UseContainerItem(const uint8& InventorySlot);

	UFUNCTION(Server, Reliable)																						// 서버_인벤토리 아이템 이동
	void Server_MoveInventoryItem(uint8 FromInventorySlot, uint8 ToInventorySlot);								

	UFUNCTION(Server, Reliable)																						// 서버_인벤토리 장비아이템 장착
	void Server_EquipFromInventory(uint8 FromInventorySlot, uint8 ToInventorySlot);

	UFUNCTION(Server, Reliable)																						// 서버_장비아이템 탈착->인벤토리 이동
	void Server_UnEquipToInventory(uint8 FromInventorySlot, uint8 ToInventorySlot);

	UFUNCTION(Server, Reliable)																						// 서버_인벤토리에서 아이템 드롭
	void Server_DropItemFromInventory(const uint8& InventorySlot);

	UFUNCTION(Server, Reliable)
	void Server_UseContainer(AActor* Container);																	// 서버_보관함 열기
	UFUNCTION(Server, Reliable)																						
	void Server_CloseContainer();																					// 서버_보관함 닫기

	UFUNCTION(Client, Reliable)																						// 클라_보관함 열기
	void Client_OpenContainer(FContainerInfo ContainerProperties, const TArray<FSlotStructure>& InContainerInventory, const TArray<FSlotStructure>& InPlayerInventory);
	UFUNCTION(Client, Reliable)																						// 클라_보관함 닫기
	void Client_CloseContainer();

	UFUNCTION(Server, Reliable)
	void Server_TakeContainerItem(const uint8& FromContainerSlot, const uint8& ToInventorySlot);					// 서버_보관함 아이템 가져오기
	UFUNCTION(Server, Reliable)
	void Server_DepositContainerItem(const uint8& FromInventorySlot, const uint8& ToInventorySlot);					// 서버_보관함에 아이템 저장
	UFUNCTION(Server, Reliable)
	void Server_MoveContainerItem(uint8 FromInventorySlot, uint8 ToInventorySlot);									// 서버_ 보관함 아이템 이동 

	UFUNCTION(Client, Reliable)
	void Client_SetInventorySlotItem(const FSlotStructure& ContentToAdd, const uint8& InventorySlot);				// 클라_인벤토리 슬롯 세팅
	UFUNCTION(Client, Reliable)
	void Client_SetContainerSlotItem(const FSlotStructure& ContentToAdd, const uint8& InventorySlot);				// 클라_보관함 슬롯 세팅

	UFUNCTION(Client, Reliable)
	void Client_ClearInventorySlotItem(uint8 InventorySlot);														// 클라_인벤토리 데이터 새로고침
	UFUNCTION(Client, Reliable)
	void Client_ClearContainerSlotItem(uint8 ContainerSlot);														// 클라_보관함 데이터 새로고침

	UFUNCTION(Client, Reliable)																						// 클라_퀵슬롯인벤 데이터 새로고침
	void Client_ClearHotbarSlot(const uint8& HotbarSlot);

	UFUNCTION(Client, Reliable)																						// 클라_퀵슬롯인벤 아이템 이동
	void Client_MoveHotbarSlotItem(const uint8& FromSlot, const uint8& ToSlot, const bool IsDraggedFromInventory, const bool IsDraggedFromHotbar);
	UFUNCTION(Client, Reliable)																						// 클라_퀵슬롯인벤 아이템 사용
	void Client_UseHotbarSlot(const uint8& HotbarSlot);

	UFUNCTION(Client, Reliable)
	void Client_LoadInventoryUI();																					// 클라_인벤토리UI 로드
	UFUNCTION(Client, Reliable)
	void Client_LoadProfileUI();																					// 클라_장비UI 로드
	UFUNCTION(Client, Reliable)
	void Client_LoadHotbarUI();																						// 클라_퀵슬롯인벤UI 로드
	UFUNCTION()
	void LoadHotbarUI();																							// 퀵슬롯인벤UI 실제로드 
	UFUNCTION(Client, Reliable)
	void Client_CheckHotbarSlots(const FSlotStructure& Slot);														// 클라_퀵슬롯인벤 슬롯 체크 

	UFUNCTION(Category = "Manager|Public")
	void InitializeInventoryManagerUI(UMainMenu* MainLayout);														// 인벤토리 모든 UI 세팅 - 메인UI

	UFUNCTION(Category = "Manager|Private")
	EEquipmentSlot GetEquipmentTypeBySlot(const uint8& EquipmentSlot);												// 장비슬롯 장비타입으로 분류해서 가져오기
	UFUNCTION()
	EItemType GetItemTypeBySlot(uint8 ItemSlot);																	// 아이템슬롯 아이템타입으로 분류해서 가져오기

	UFUNCTION(Category = "Manager|Private|Inventory")
	void AddItem(UInventoryComponent* Inventory, uint8 InventorySlot, FSlotStructure& InventoryItem);				// 슬롯에 아이템 추가

	UFUNCTION()
	FSlotStructure GetEmptySlot(EEquipmentSlot FromEquipmentType);													// 비어있는 장비슬롯 가져오기

	FSlotStructure GetItemFromItemDB(FName Name);																	// 아이템 데이터 아이템 이름으로 가져오기

	UFUNCTION()
	void UpdateEquippedStats();																						// 장비 착용 상태 업데이트

	UFUNCTION(Category = "Manager|Getter")
	UDataTable* GetItemDB();																						// 아이템 데이터테이블 가져오기 

	UPROPERTY()
	AActor* CurrentContainer;																						// 보관함액터 

	// 슬롯개수 설정 //
	uint8 NumberOfRowsInventory = 0;
	uint8 SlotsPerRowInventory = 0;
	uint8 NumberOfSlotsOnHotbar = 0;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void InitializeInventoryManager(UInventoryComponent* EquipmentComponent);		// 인벤매니저 세팅

	UFUNCTION()
	virtual void Server_InitInventory(); // 서버RPC_인벤토리 추가 
		
private:
	
	UFUNCTION(Category = "UserInterface|Private|Hotbar")															// 퀵슬롯인벤 아이템 이동 
	void MoveHotbarSlotItem(const uint8& FromSlot, const uint8& ToSlot, const bool IsDraggedFromInventory, const bool IsDraggedFromHotbar);
	UFUNCTION(Category = "UserInterface|Private|Hotbar")
	void UseHotbarSlot(const uint8& HotbarSlot);																	// 퀵슬롯인벤 아이템 사용
	UFUNCTION(Category = "UserInterface|Private|Hotbar")
	void ClearHotbarSlotItem(const uint8& HotbarSlot);																// 퀵슬롯인벤 데이터 새로고침

	UFUNCTION(Category = "UserInterface|Private|Hotbar")						
	void SetHotbarSlotItem(const uint8& ToSlot, FSlotStructure SlotStructure);										// 퀵슬롯인벤 데이터 세팅
	UFUNCTION(Category = "UserInterface|Private|Hotbar")
	FSlotStructure GetHotbarSlotItem(const uint8& HotbarSlot);														// 퀵슬롯인벤 아이템 데이터 받아오기

	UPROPERTY()
	UDataTable* ItemDB; // 아이템 데이터테이블 변수 

	UFUNCTION(Category = "Helper")
	uint8 GetEquipmentSlotByType(EEquipmentSlot EquipmentSlot);								// 장비타입으로 장비슬롯 세팅
	UFUNCTION(Category = "Helper")
	EEquipmentSlot GetItemEquipmentSlot(FSlotStructure Item);								// 장비슬롯 장비아이템 가져오기
	UFUNCTION(Category = "Helper")
	bool ItemIsValid(FSlotStructure Item);													// 아이템 유효성 검사
	UFUNCTION(Category = "Helper")
	bool GetEmptyEquipmentSlotByType(EEquipmentSlot EquipmentSlot, uint8& OutIndex);		// 장비 빈슬롯 가져오기
	UFUNCTION(Category = "Helper")
	bool ItemIsSame(const FSlotStructure Item1, const FSlotStructure Item2);				// 슬롯에서 아이템 같은건지 확인
	UFUNCTION(Category = "Helper")
	bool IsItemStackable(const FSlotStructure Item);										// 아이템 적재보관 가능한지 확인
	UFUNCTION(Category = "Helper")
	uint8 ItemFreeStackSpace(const FSlotStructure Item);									// 아이템 보관가능 슬롯 찾기
	UFUNCTION(Category = "Helper")
	uint8 GetItemAmount(const FSlotStructure Item);											// 아이템 갯수 가져오기
	UFUNCTION(Category = "Helper")
	uint8 GetItemMaxStackSize(const FSlotStructure Item);									// 아이템 최대 적재보관 개수 가져오기
	UFUNCTION(Category = "Helper")
	void AddAmountToItem(FSlotStructure& Item, uint8 AmountToAdd);							// 아이템 추가개수 만큼 추가 

	UFUNCTION()																				// 아이템 일정개수 지우기 
	void RemoveFromItemAmount(FSlotStructure& InventoryItem, const uint8& AmountToRemove, bool& WasFullAmountRemoved, uint8& AmountRemoved);
	UFUNCTION(Category = "Manager|Private|Equipment")										// 아이템 장착
	void EquipItem(UInventoryComponent* FromInventory, uint8 FromInventorySlot, UInventoryComponent* ToInventory, uint8 ToInventorySlot);
	UFUNCTION(Category = "Manager|Private|Equipment")										// 아이템 탈착
	void UnEquipItem(UInventoryComponent* FromInventory, uint8 FromInventorySlot,UInventoryComponent* ToInventory, uint8 ToInventorySlot);
	UFUNCTION(Category = "Manager|Private")													// 아이템 버리는 위치 지정
	void RandomizeDropLocation(FSlotStructure LocalSlot, UClass*& LocalClass, FTransform& OutTransform);
	UFUNCTION(Category = "Manager|Private|Inventory")										// 아이템 버리기
	void DropItem(UInventoryComponent* Inventory, uint8 InventorySlot);

	UFUNCTION(Category = "Manager|Private|Inventory")										// 아이템 옮기기
	void MoveItem(UInventoryComponent* FromInventory, uint8 FromInventorySlot, UInventoryComponent* ToInventory, uint8 ToInventorySlot);
	UFUNCTION(Category = "Manager|Private|Inventory")										// 인벤토리에서 아이템 지우기 
	void RemoveItem(UInventoryComponent* Inventory, uint8 InventorySlot);

	UFUNCTION(Category = "Manager|Private|Items")																 
	void UseEquipmentItem(uint8 InventorySlot, FSlotStructure InventoryItem, UInventoryComponent* ToInventory);	// 장비아이템 사용
	UFUNCTION(Category = "Manager|Private|Items")
	void UseConsumableItem(uint8 InventorySlot, FSlotStructure InventoryItem);									// 소비아이템 사용
	UFUNCTION(Category = "Manager|Private|Items")
	void UseFunitureItem(uint8 InventorySlot, FSlotStructure InventoryItem);									// 설치아이템(가구) 사용
	UFUNCTION(Category = "Manager|Private|Items")
	void UseInventoryItem(const uint8& InventorySlot);															// 인벤토리의 아이템 사용(전체)

	UFUNCTION(Category = "UserInterface|Private|Inventory")
	void ClearInventoryItem(const uint8& InventorySlot);														// 인벤토리 아이템 새로고침
	UFUNCTION(Category = "UserInterface|Private|Inventory")
	void SetInventorySlotItem(const FSlotStructure& ContentToAdd, const uint8& InventorySlot);					// 슬롯에 할당된 아이템 세팅
	UFUNCTION(Category = "UserInterface|Private|Inventory")
	void ClearInventorySlotItem(uint8 InventorySlot);															// 슬롯에 할당된 아이템 새로고침 
	UFUNCTION(Category = "UserInterface|Private|Inventory")
	FSlotStructure GetInventorySlotItem(uint8 InventorySlot);													// 슬롯에 할당'될' 아이템 가져오기

	UFUNCTION(Category = "Manager|Private|Inventory")
	void AddGold(uint8 Amount);																					// 돈 추가

	UFUNCTION(Category = "UserInterface|Private|Container")
	void ClearContainerSlots();																					// 보관함 슬롯 새로고침
	UFUNCTION(Category = "UserInterface|Private|Container")
	void CreateContainerSlots(uint8 NumberOfRows, uint8 SlotsPerRow);											// 보관함 슬롯 생성
	UFUNCTION(Category = "UserInterface|Private|Container")
	void SetViewersContainerSlot(uint8 ContainerSlot, FSlotStructure& InventoryItem);							// 눈에 보일 보관함 슬롯 생성

	UFUNCTION(Category = "UserInterface|Private|Container")
	void ClearContainerSlotItem(uint8 ContainerSlot);															// 보관함 슬롯 아이템 새로고침
	UFUNCTION(Category = "UserInterface|Private|Container")
	void ClearViewersContainerSlot(uint8 ContainerSlot);														// 눈에 보이는 보관함 슬롯 -> 새로고침
	UFUNCTION(Category = "UserInterface|Private|Container")
	void SetContainerSlotItem(const FSlotStructure& Slot, uint8 Index);											// 보관함 슬롯 할당'될' 아이템 가져오기

	UFUNCTION(Category = "Manager|Private|Container")
	void OpenContainer(AActor* Container);																		// 보관함 열기
	UFUNCTION(Category = "Manager|Private|Container")
	void UseContainer(AActor* Container);																		// 보관함 사용
	UFUNCTION(Category = "Manager|Private|Container")
	void CloseContainer();																						// 보관함 닫기
	UFUNCTION(Category = "Manager|Private|Container")															// 보관함 슬롯 로드 
	void LoadContainerSlots(FContainerInfo ContainerProperties, const TArray<FSlotStructure>& InContainerInventory, const TArray<FSlotStructure>& InPlayerInventory);
	UFUNCTION(Category = "Manager|Private")																		// 보관함에 아이템 저장 가능한지 확인
	bool CanContainerStoreItems(UInventoryComponent* Inventory);

	UFUNCTION(Category = "Manager|Private|Stacks")																// 아이템 추가 적재보관 가능한 수 확인
	void FindAndAddAmountToStacks(UInventoryComponent* Inventory, FName ItemID, uint8 Amount, uint8& AmountRemaining);

	UFUNCTION(Category = "Manager|Private|Stacks")																// 적재보관 수 
	void AddItemToStack(UInventoryComponent* Inventory, uint8 InventorySlot, uint8 AmountToAdd, uint8& AmountRemaining);

};
