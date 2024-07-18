

#include "InventoryManagerComponent.h"
#include "ContainerActor.h"
#include "EAttributes.h"
#include "FWidgetsLayoutBP.h"
#include "MyHUD.h"
#include "WorldActor.h"
#include "LootActor.h"
#include "EquipmentComponent.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "MainLayout.h"
#include "ContainerLayout.h"
#include "InventoryLayout.h"
#include "ProfileLayout.h"
#include "Hotbar.h"
#include "W_ItemTooltip.h"
#include "SlotLayout.h"
#include "Hotbar_Slot.h"
#include "GameFramework/PlayerState.h"
#include "FContainerInfo.h"
#include "Net/UnrealNetwork.h"
#include "StateComponent.h"
#include "VICTIMSCharacter.h"
#include "AVICTIMSPlayerController.h"
#include "HUDLayout.h"
#include "Character/CharacterBase.h"
#include "HousingComponent.h"
#include "ShopLayout.h"
#include "ShopActor.h"
#include "Components/ScrollBox.h"
#include "Shop_Slo.h"
#include "Inventory/InventoryComponent.h"

UInventoryManagerComponent::UInventoryManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}
void UInventoryManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	Gold = 0;
	TotalNumberOfSlots = (NumberOfRowsInventory * SlotsPerRowInventory) + (uint8)EEquipmentSlot::Count;
	InitializeItemDB();

	if (GetOwner())
	{
		auto ownerController = Cast<AVICTIMSPlayerController>(GetOwner());

		if (ownerController && ownerController->IsLocalController())
		{
			HUD = GetPlayerHud();
			auto ownerCharacter = ownerController->GetPawn();
			if (ownerCharacter)
			{
				playerReference = Cast<AVICTIMSCharacter>(ownerCharacter);
				UE_LOG(LogTemp, Warning, TEXT("111111111111111111111111111111111111111111111111111"));
			}
		}
	}


}


void UInventoryManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UInventoryManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryManagerComponent, TotalNumberOfSlots);
	DOREPLIFETIME(UInventoryManagerComponent, AttributesArray);
	DOREPLIFETIME(UInventoryManagerComponent, Gold);
}

void UInventoryManagerComponent::OnRep_UpdateGoldAmount()
{
	if (IsValid(MainLayoutUI))
	{
		MainLayoutUI->Inventory->UpdateGoldAmount();
	}
}

void UInventoryManagerComponent::InitializeItemDB()
{
	UDataTable* BP_ItemDB = LoadObject<UDataTable>(this, TEXT("/Script/Engine.DataTable'/Game/Item/Data/Item_DB.Item_DB'"));
	if (IsValid(BP_ItemDB))
	{
		ItemDB = BP_ItemDB;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UDataTable not Loaded"))
	}
}

void UInventoryManagerComponent::Server_InitInventory_Implementation()
{
	PlayerInventory->Server_InitInventory(TotalNumberOfSlots);
}
void UInventoryManagerComponent::InitializeInventoryManager(UInventoryComponent* EquipmentComponent)
{
	PlayerInventory = EquipmentComponent;
}

void UInventoryManagerComponent::TryToAddItemToInventory(UInventoryComponent* Inventory, FSlotStructure& InventoryItem, bool& bOutSuccess)
{
	FName LocalItemID = InventoryItem.ItemStructure.ID;
	uint8 LocalItemAmount = InventoryItem.Amount;
	FSlotStructure LocalInventoryItem = InventoryItem;
	UInventoryComponent* LocalInventory = Inventory;

	uint8 AmountRemaining = LocalItemAmount;

	if (LocalInventoryItem.ItemStructure.ItemType == EItemType::Currency)
	{
		AddGold(LocalInventoryItem.Amount);
		bOutSuccess = true;
		return;
	}

	if (LocalInventoryItem.ItemStructure.IsStackable)
	{
		FindAndAddAmountToStacks(LocalInventory, LocalItemID, LocalItemAmount, AmountRemaining);
		LocalItemAmount = AmountRemaining;
		if (LocalItemAmount == 0)
		{
			bOutSuccess = true;
			return;
		}
		else
		{
			LocalInventoryItem.Amount = LocalItemAmount;
		}
	}

	FReturnTupleBoolInt LocalTuple;
	if (LocalInventory == PlayerInventory)
	{
		LocalTuple = LocalInventory->GetEmptyInventorySpace();
	}
	else if (LocalInventory == ContainerInventory)
	{
		LocalTuple = LocalInventory->GetEmptyContainerSpace();
	}
	if (LocalTuple.Success)
	{
		AddItem(Inventory, LocalTuple.Index, LocalInventoryItem);
		bOutSuccess = true;
		return;
	}
	else
	{
		UE_LOG(LogTemp, Verbose, TEXT("Inventory Full"))
			bOutSuccess = false;
		return;
	}
}

void UInventoryManagerComponent::Server_UseInventoryItem_Implementation(const uint8& InventorySlot)
{
	UseInventoryItem(InventorySlot);
}

void UInventoryManagerComponent::Server_UseContainerItem_Implementation(const uint8& InventorySlot)
{
	UseContainerItem(InventorySlot);
}

void UInventoryManagerComponent::Server_MoveInventoryItem_Implementation(uint8 FromInventorySlot, uint8 ToInventorySlot)
{
	MoveItem(PlayerInventory, FromInventorySlot, PlayerInventory, ToInventorySlot);
}

void UInventoryManagerComponent::Server_EquipFromInventory_Implementation(uint8 FromInventorySlot, uint8 ToInventorySlot)
{
	EquipItem(PlayerInventory, FromInventorySlot, PlayerInventory, ToInventorySlot);
}

void UInventoryManagerComponent::Server_UnEquipToInventory_Implementation(uint8 FromInventorySlot, uint8 ToInventorySlot)
{
	UnEquipItem(PlayerInventory, FromInventorySlot, PlayerInventory, ToInventorySlot);
}

void UInventoryManagerComponent::Server_DropItemFromInventory_Implementation(const uint8& InventorySlot)
{
	DropItem(PlayerInventory, InventorySlot);
}

void UInventoryManagerComponent::Server_UseContainer_Implementation(AActor* Container)
{
	UseContainer(Container);
}

void UInventoryManagerComponent::Server_CloseContainer_Implementation()
{
	CloseContainer();
}

void UInventoryManagerComponent::Server_TakeContainerItem_Implementation(const uint8& FromContainerSlot, const uint8& ToInventorySlot)
{
	MoveItem(ContainerInventory, FromContainerSlot, PlayerInventory, ToInventorySlot);
}

void UInventoryManagerComponent::Server_DepositContainerItem_Implementation(const uint8& FromInventorySlot, const uint8& ToInventorySlot)
{
	MoveItem(PlayerInventory, FromInventorySlot, ContainerInventory, ToInventorySlot);
}

void UInventoryManagerComponent::Server_MoveContainerItem_Implementation(uint8 FromInventorySlot, uint8 ToInventorySlot)
{
	MoveItem(ContainerInventory, FromInventorySlot, ContainerInventory, ToInventorySlot);
}

void UInventoryManagerComponent::Server_EquipFromContainer_Implementation(uint8 FromInventorySlot, uint8 ToInventorySlot)
{
	EquipItem(ContainerInventory, FromInventorySlot, PlayerInventory, ToInventorySlot);
	Server_UpdateTooltips();
}

void UInventoryManagerComponent::Server_UnEquipToContainer_Implementation(uint8 FromInventorySlot, uint8 ToInventorySlot)
{
	UnEquipItem(PlayerInventory, FromInventorySlot, ContainerInventory, ToInventorySlot);
	Server_UpdateTooltips();
}

void UInventoryManagerComponent::Client_SetInventorySlotItem_Implementation(const FSlotStructure& ContentToAdd, const uint8& InventorySlot)
{
	SetInventorySlotItem(ContentToAdd, InventorySlot);
}

void UInventoryManagerComponent::Client_SetContainerSlotItem_Implementation(const FSlotStructure& ContentToAdd, const uint8& InventorySlot)
{
	SetContainerSlotItem(ContentToAdd, InventorySlot);
}

void UInventoryManagerComponent::Client_ClearInventorySlotItem_Implementation(uint8 InventorySlot)
{
	ClearInventorySlotItem(InventorySlot);
}

void UInventoryManagerComponent::Client_ClearContainerSlotItem_Implementation(uint8 ContainerSlot)
{
	ClearContainerSlotItem(ContainerSlot);
}

void UInventoryManagerComponent::Client_OpenContainer_Implementation(FContainerInfo ContainerProperties, const TArray<FSlotStructure>& InContainerInventory, const TArray<FSlotStructure>& InPlayerInventory)
{
	LoadContainerSlots(ContainerProperties, InContainerInventory, InPlayerInventory);
}

void UInventoryManagerComponent::Client_CloseContainer_Implementation()
{
	MainLayoutUI->Container->SetVisibility(ESlateVisibility::Hidden);
}

void UInventoryManagerComponent::Client_MoveHotbarSlotItem_Implementation(const uint8& FromSlot, const uint8& ToSlot, const bool IsDraggedFromInventory, const bool IsDraggedFromHotbar)
{
	MoveHotbarSlotItem(FromSlot, ToSlot, IsDraggedFromInventory, IsDraggedFromHotbar);
}

void UInventoryManagerComponent::Client_UseHotbarSlot_Implementation(const uint8& HotbarSlot)
{
	UseHotbarSlot(HotbarSlot);
}

void UInventoryManagerComponent::Client_ClearHotbarSlot_Implementation(const uint8& HotbarSlot)
{
	ClearHotbarSlotItem(HotbarSlot);
}

void UInventoryManagerComponent::Server_UpdateTooltips_Implementation()
{
	if (IsValid(ContainerInventory))
	{
		Client_UpdateContainerTooltips(PlayerInventory->Inventory, ContainerInventory->Inventory);
	}
	Client_UpdateInventoryTooltips(PlayerInventory->Inventory, PlayerInventory->Inventory);
}

void UInventoryManagerComponent::Client_LoadInventoryUI_Implementation()
{
	AVICTIMSPlayerController* PC = Cast<AVICTIMSPlayerController>(GetOwner());

	FWidgetsLayoutBP* WidgetLayout = Cast<AMyHUD>(PC->HUD_Reference)->GetWidgetBPClass("SlotLayout_WBP");
	if (WidgetLayout)
	{
		USlotLayout* W_Slot = nullptr;

		for (int i = 0; i < NumberOfRowsInventory; i++)
		{
			for (int j = 0; j < SlotsPerRowInventory; j++)
			{
				W_Slot = CreateWidget<USlotLayout>(GetWorld(), WidgetLayout->Widget);
				MainLayoutUI->Inventory->InventoryGridPanel->AddChildToUniformGrid(W_Slot, i, j);

				MainLayoutUI->Inventory->InventorySlotsArray.Add(W_Slot);
			}
		}

		FSlotStructure SlotStructure = GetEmptySlot(EEquipmentSlot::Undefined);

		USlotLayout* LocalSlot{};
		for (int i = 0; i < MainLayoutUI->Inventory->InventorySlotsArray.Num(); i++)
		{
			LocalSlot = MainLayoutUI->Inventory->InventorySlotsArray[i];

			LocalSlot->SetSlotIndex(i + (uint8)EEquipmentSlot::Count);
			LocalSlot->NativeFromInventory = true;

			LocalSlot->UpdateSlot(SlotStructure);
		}
	}
}

void UInventoryManagerComponent::Client_LoadProfileUI_Implementation()
{
	AVICTIMSPlayerController* PC = Cast<AVICTIMSPlayerController>(GetOwner());

	FWidgetsLayoutBP* WidgetLayout = Cast<AMyHUD>(PC->HUD_Reference)->GetWidgetBPClass("SlotLayout_WBP");
	if (WidgetLayout)
	{
		USlotLayout* W_Slot = nullptr;

		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				W_Slot = CreateWidget<USlotLayout>(GetWorld(), WidgetLayout->Widget);

				UUniformGridSlot* GridSlot = MainLayoutUI->Profile->EquipmentGridPanel->AddChildToUniformGrid(W_Slot, i, j);
				GridSlot->SetHorizontalAlignment(HAlign_Center);
				GridSlot->SetVerticalAlignment(VAlign_Center);

				MainLayoutUI->Profile->EquipmentSlotsArray.Add(W_Slot);
			}
		}

		FSlotStructure SlotStructure{};
		USlotLayout* LocalSlot{};

		for (int i = 0; i < MainLayoutUI->Profile->EquipmentSlotsArray.Num(); i++)
		{
			if (i == 0)
			{
				SlotStructure = GetEmptySlot(EEquipmentSlot::Weapon);
			}
			else if (i == 1)
			{
				SlotStructure = GetEmptySlot(EEquipmentSlot::Chest);
			}
			else if (i == 2)
			{
				SlotStructure = GetEmptySlot(EEquipmentSlot::Feet);
			}
			else if (i == 3)
			{
				SlotStructure = GetEmptySlot(EEquipmentSlot::Hands);
			}
			else if (i == 4)
			{
				SlotStructure = GetEmptySlot(EEquipmentSlot::Legs);
			}
			else if (i == 5)
			{
				SlotStructure = GetEmptySlot(EEquipmentSlot::Head);
			}
			else {
				SlotStructure = GetEmptySlot(EEquipmentSlot::Undefined);
			}

			LocalSlot = MainLayoutUI->Profile->EquipmentSlotsArray[i];

			LocalSlot->SetSlotIndex(i);
			LocalSlot->NativeFromInventory = true;

			LocalSlot->UpdateSlot(SlotStructure);
		}
	}
}

void UInventoryManagerComponent::Client_LoadHotbarUI_Implementation()
{
	LoadHotbarUI();
}

void UInventoryManagerComponent::Client_CheckHotbarSlots_Implementation(const FSlotStructure& Slot)
{
	bool Success = false;
	TArray<USlotLayout*> LocalInventoryUI2 = MainLayoutUI->Inventory->InventorySlotsArray;
	for (uint8 i = 0; i < LocalInventoryUI2.Num(); i++)
	{
		if (Slot.ItemStructure.ID == LocalInventoryUI2[i]->SlotStructure.ItemStructure.ID)
		{
			Success = true;
			break;
		}
	}
	if (!Success)
	{
		TArray<UHotbar_Slot*> Hotbar = MainLayoutUI->Hotbar->HotbarSlotsArray;

		for (uint8 i = 0; i < Hotbar.Num(); i++) {
			{
				if (Slot.ItemStructure.ID == GetHotbarSlotItem(i).ItemStructure.ID)
				{
					ClearHotbarSlotItem(i);
					break;
				}
			}
		}
	}
}

void UInventoryManagerComponent::LoadHotbarUI()
{
	AVICTIMSPlayerController* PC = Cast<AVICTIMSPlayerController>(GetOwner());

	FWidgetsLayoutBP* WidgetLayout = Cast<AMyHUD>(PC->HUD_Reference)->GetWidgetBPClass("HotbarSlot_WBP");
	if (WidgetLayout)
	{
		UHotbar_Slot* W_Slot = nullptr;
		uint8 Row = 0;
		uint8 Column = 0;

		uint8 KeyNumber = 0;
		for (uint8 i = 0; i < NumberOfSlotsOnHotbar; i++)
		{
			Column = i;

			W_Slot = CreateWidget<UHotbar_Slot>(GetWorld(), WidgetLayout->Widget);

			KeyNumber = i;
			if (KeyNumber == PC->GetMaximumHotbarSlots())
			{
				KeyNumber = PC->GetMaximumHotbarSlots();
			}

			W_Slot->SetKeyNumber(KeyNumber);

			MainLayoutUI->Hotbar->HotbarGridPanel->AddChildToUniformGrid(W_Slot, Row, Column);
			MainLayoutUI->Hotbar->HotbarSlotsArray.Add(W_Slot);
		}

		FSlotStructure SlotStructure = GetEmptySlot(EEquipmentSlot::Undefined);

		UHotbar_Slot* LocalSlot{};
		for (int i = 0; i < MainLayoutUI->Hotbar->HotbarSlotsArray.Num(); i++)
		{
			LocalSlot = MainLayoutUI->Hotbar->HotbarSlotsArray[i];

			LocalSlot->SetSlotIndex(i);
			LocalSlot->NativeFromHotbar = true;

			LocalSlot->UpdateSlot(SlotStructure);
		}
	}
}

void UInventoryManagerComponent::InitializeInventoryManagerUI(UMainLayout* MainLayout)
{
	MainLayoutUI = MainLayout;
}

EEquipmentSlot UInventoryManagerComponent::GetEquipmentTypeBySlot(const uint8& EquipmentSlot)
{
	switch (EquipmentSlot)
	{
	case 0:
		return EEquipmentSlot::Weapon;
	case 1:
		return EEquipmentSlot::Chest;
	case 2:
		return EEquipmentSlot::Feet;
	case 3:
		return EEquipmentSlot::Hands;
	case 4:
		return EEquipmentSlot::Legs;
	case 5:
		return EEquipmentSlot::Head;
	default:
		return EEquipmentSlot::Undefined;
	}
}

EItemType UInventoryManagerComponent::GetItemTypeBySlot(uint8 ItemSlot)
{
	return PlayerInventory->Inventory[ItemSlot].ItemStructure.ItemType;
}

void UInventoryManagerComponent::AddItem(UInventoryComponent* Inventory, uint8 InventorySlot, FSlotStructure& InventoryItem)
{
	Inventory->SetInventoryItem(InventorySlot, InventoryItem);

	if (PlayerInventory == Inventory)
	{
		Client_SetInventorySlotItem(InventoryItem, InventorySlot);
	}
	else
	{
		SetViewersContainerSlot(InventorySlot, InventoryItem);
	}
}

FSlotStructure UInventoryManagerComponent::GetEmptySlot(EEquipmentSlot FromEquipmentType)
{
	FName Name;
	if (FromEquipmentType == EEquipmentSlot::Weapon)
	{
		Name = "No_Weapon";
	}
	else if (FromEquipmentType == EEquipmentSlot::Chest)
	{
		Name = "No_Chest";
	}
	else if (FromEquipmentType == EEquipmentSlot::Feet)
	{
		Name = "No_Feet";
	}
	else if (FromEquipmentType == EEquipmentSlot::Hands)
	{
		Name = "No_Hands";
	}
	else if (FromEquipmentType == EEquipmentSlot::Legs)
	{
		Name = "No_Legs";
	}
	else if (FromEquipmentType == EEquipmentSlot::Head)
	{
		Name = "No_Helmet";
	}
	else
	{
		Name = "Empty";
	}
	return GetItemFromItemDB(Name);
}

FSlotStructure UInventoryManagerComponent::GetItemFromItemDB(FName Name)
{
	FSlotStructure Slot = {};

	if (IsValid(ItemDB))
	{
		const UDataTable* ItemTable = ItemDB;
		const FItemStructure* NewItemData = ItemTable->FindRow<FItemStructure>(Name, "", true);

		Slot.InitSlot(*NewItemData, 0);
	}
	else {
		InitializeItemDB();

		if (IsValid(ItemDB))
		{
			const UDataTable* ItemTable = ItemDB;
			const FItemStructure* NewItemData = ItemTable->FindRow<FItemStructure>(Name, "", true);

			Slot.InitSlot(*NewItemData, 0);
		}
	}
	return Slot;
}

void UInventoryManagerComponent::Client_SetAttributes_Implementation(int32 Value)
{
	SetAttributes(Value);
}

void UInventoryManagerComponent::UpdateEquippedStats()
{
	uint8 TempStrength = 0;
	for (int i = 0; i < (uint8)EEquipmentSlot::Count; i++)
	{
		FSlotStructure TempSlot = PlayerInventory->GetInventoryItem(i);

		TempStrength += TempSlot.ItemStructure.Strength;

	}
	Client_SetAttributes(TempStrength);
}

UDataTable* UInventoryManagerComponent::GetItemDB()
{
	return ItemDB;
}

void UInventoryManagerComponent::InitializePlayerAttributes()
{
	AttributesArray.Init(0, (uint8)EAttributes::Count);
}

void UInventoryManagerComponent::Client_UpdateInventoryTooltips_Implementation(const TArray<FSlotStructure>& InPlayerInventory, const TArray<FSlotStructure>& InOtherInventory)
{
	AVICTIMSPlayerController* PC = Cast<AVICTIMSPlayerController>(GetOwner());
	if (!IsValid(PC))
	{
		return;
	}
	FWidgetsLayoutBP* WidgetLayout = Cast<AMyHUD>(PC->HUD_Reference)->GetWidgetBPClass("ItemTooltip_WBP");

	UW_ItemTooltip* Tooltip{};
	FSlotStructure TempSlot{};
	USlotLayout* SlotLayout{};

	uint8 Index = 0;
	uint8 LocalIndex = 0;

	for (FSlotStructure Slot : InOtherInventory)
	{
		TempSlot = GetEmptySlot(EEquipmentSlot::Undefined);

		// Get SlotLayout Reference
		if (Index < (uint8)EEquipmentSlot::Count)
		{
			SlotLayout = MainLayoutUI->Profile->EquipmentSlotsArray[Index];
		}
		else
		{
			LocalIndex = Index - (uint8)EEquipmentSlot::Count;
			SlotLayout = MainLayoutUI->Inventory->InventorySlotsArray[LocalIndex];
		}
		Tooltip = Cast<UW_ItemTooltip>(SlotLayout->GetToolTip());
		if (!IsValid(Tooltip))
		{
			Tooltip = CreateWidget<UW_ItemTooltip>(GetWorld(), WidgetLayout->Widget);
// 			UE_LOG(LogTemp, Verbose, TEXT("Creating Inventory Tooltip"))
		}

		if (Slot.ItemStructure.ItemType == EItemType::Equipment)
		{
			for (uint8 j = 0; j < (uint8)EEquipmentSlot::Count; j++)
			{
				if (GetItemEquipmentSlot(InPlayerInventory[j]) == GetItemEquipmentSlot(Slot))
				{
					TempSlot = InPlayerInventory[j];
				}
			}
		}

		Tooltip->UpdateTooltipAttributes(Slot.ItemStructure, TempSlot);
		SlotLayout->ToggleTooltip();
		SlotLayout->SetToolTip(Tooltip);

		Index++;
	}

	int iTemp = 0;
}

void UInventoryManagerComponent::Client_UpdateContainerTooltips_Implementation(const TArray<FSlotStructure>& InPlayerInventory, const TArray<FSlotStructure>& InOtherInventory)
{
	AVICTIMSPlayerController* PC = Cast<AVICTIMSPlayerController>(GetOwner());
	if (!IsValid(PC))
	{
		return;
	}
	FWidgetsLayoutBP* WidgetLayout = Cast<AMyHUD>(PC->HUD_Reference)->GetWidgetBPClass("ItemTooltip_WBP");

	UW_ItemTooltip* Tooltip{};
	FSlotStructure TempSlot{};
	USlotLayout* SlotLayout{};

	uint8 Index = 0;
	uint8 LocalIndex = 0;

	for (FSlotStructure Slot : InOtherInventory)
	{
		TempSlot = GetEmptySlot(EEquipmentSlot::Undefined);
		SlotLayout = MainLayoutUI->Container->ContainerSlotsArray[Index];

		Tooltip = Cast<UW_ItemTooltip>(SlotLayout->GetToolTip());
		if (!IsValid(Tooltip))
		{
			Tooltip = CreateWidget<UW_ItemTooltip>(GetWorld(), WidgetLayout->Widget);
// 			UE_LOG(LogTemp, Verbose, TEXT("Creating Container Tooltip"))
		}

		if (Slot.ItemStructure.ItemType == EItemType::Equipment)
		{
			for (uint8 j = 0; j < (uint8)EEquipmentSlot::Count; j++)
			{
				if (GetItemEquipmentSlot(InPlayerInventory[j]) == GetItemEquipmentSlot(Slot))
				{
					TempSlot = InPlayerInventory[j];
					break;
				}
			}
		}

		Tooltip->UpdateTooltipAttributes(Slot.ItemStructure, TempSlot);
		SlotLayout->ToggleTooltip();
		SlotLayout->SetToolTip(Tooltip);

		Index++;
	}
}

void UInventoryManagerComponent::MoveHotbarSlotItem(const uint8& FromSlot, const uint8& ToSlot, const bool IsDraggedFromInventory, const bool IsDraggedFromHotbar)
{
	if (IsDraggedFromInventory)
	{
		FSlotStructure SlotStructure = GetInventorySlotItem(FromSlot);

		SetHotbarSlotItem(ToSlot, SlotStructure);
	}

	if (IsDraggedFromHotbar)
	{
		FSlotStructure FromSlotItem = GetHotbarSlotItem(FromSlot);
		FSlotStructure ToSlotItem = GetHotbarSlotItem(ToSlot);

		if (ItemIsValid(ToSlotItem))
		{
			SetHotbarSlotItem(ToSlot, FromSlotItem);
			SetHotbarSlotItem(FromSlot, ToSlotItem);
		}
		else {
			SetHotbarSlotItem(ToSlot, FromSlotItem);
			ClearHotbarSlotItem(FromSlot);
		}
	}
}

void UInventoryManagerComponent::UseHotbarSlot(const uint8& HotbarSlot)
{
	FReturnTupleBoolInt Tuple{};
	FSlotStructure Slot = GetHotbarSlotItem(HotbarSlot);

	if (ItemIsValid(Slot))
	{
		TArray<USlotLayout*> LocalInventoryUI = MainLayoutUI->Inventory->InventorySlotsArray;
		for (uint8 i = 0; i < LocalInventoryUI.Num(); i++) {
			if (Slot.ItemStructure.ID == LocalInventoryUI[i]->SlotStructure.ItemStructure.ID) {
				Tuple.Success = true;
				Tuple.Index = i;
				break;
			}
		}

		if (Tuple.Success)
		{
			Tuple.Index = Tuple.Index + (uint8)EEquipmentSlot::Count;
			Server_UseInventoryItem(Tuple.Index);
		}
	}
}

void UInventoryManagerComponent::ClearHotbarSlotItem(const uint8& HotbarSlot)
{
	UHotbar_Slot* Slot = MainLayoutUI->Hotbar->HotbarSlotsArray[HotbarSlot];
	FSlotStructure LocalSlot = GetEmptySlot(EEquipmentSlot::Undefined);
	Slot->UpdateSlot(LocalSlot);
}

void UInventoryManagerComponent::SetHotbarSlotItem(const uint8& ToSlot, FSlotStructure SlotStructure)
{
	UHotbar_Slot* HotbarSlot = MainLayoutUI->Hotbar->HotbarSlotsArray[ToSlot];
	HotbarSlot->UpdateSlot(SlotStructure);
}

FSlotStructure UInventoryManagerComponent::GetHotbarSlotItem(const uint8& HotbarSlot)
{
	TArray<UHotbar_Slot*> HotbarSlotsArray = MainLayoutUI->Hotbar->HotbarSlotsArray;
	FSlotStructure Slot = HotbarSlotsArray[HotbarSlot]->SlotStructure;
	return Slot;
}

uint8 UInventoryManagerComponent::GetEquipmentSlotByType(EEquipmentSlot EquipmentSlot)
{
	for (uint8 Index = 0; Index < (uint8)EEquipmentSlot::Count; Index++)
	{
		EEquipmentSlot LocalSlotType = GetEquipmentTypeBySlot(Index);
		if (EquipmentSlot == LocalSlotType)
		{
			return Index;
		}
	}
	return 0;
}

EEquipmentSlot UInventoryManagerComponent::GetItemEquipmentSlot(FSlotStructure LocalInventoryItem)
{
	return LocalInventoryItem.ItemStructure.EquipmentSlot;
}

bool UInventoryManagerComponent::ItemIsValid(FSlotStructure Item)
{
	if (Item.Amount > 0)
	{
		return true;
	}
	return false;
}

bool UInventoryManagerComponent::GetEmptyEquipmentSlotByType(EEquipmentSlot EquipmentSlot, uint8& OutIndex)
{
	for (uint8 Index = 0; Index < (uint8)EEquipmentSlot::Count; Index++)
	{
		EEquipmentSlot LocalEquipSlot = GetEquipmentTypeBySlot(Index);
		FSlotStructure LocalSlot = PlayerInventory->GetInventorySlot(Index);

		if (!ItemIsValid(LocalSlot) && EquipmentSlot == LocalEquipSlot)
		{
			OutIndex = Index;
			return true;
		}
	}
	return false;
}

bool UInventoryManagerComponent::ItemIsSame(const FSlotStructure Item1, const FSlotStructure Item2)
{
	if (Item1.ItemStructure.ID == Item2.ItemStructure.ID)
	{
		return true;
	}
	return false;
}

bool UInventoryManagerComponent::IsItemStackable(const FSlotStructure Item)
{
	return Item.ItemStructure.IsStackable;
}

uint8 UInventoryManagerComponent::ItemFreeStackSpace(const FSlotStructure Item)
{
	return GetItemMaxStackSize(Item) - GetItemAmount(Item);
}

uint8 UInventoryManagerComponent::GetItemAmount(const FSlotStructure Item)
{
	return Item.Amount;
}

uint8 UInventoryManagerComponent::GetItemMaxStackSize(const FSlotStructure Item)
{
	return Item.ItemStructure.MaxStackSize;
}

void UInventoryManagerComponent::AddAmountToItem(FSlotStructure& Item, uint8 AmountToAdd)
{
	Item.Amount += AmountToAdd;
}

void UInventoryManagerComponent::SetAttributes(int32 Value)
{
	{
		if (IsValid(MainLayoutUI))
		{
			MainLayoutUI->Profile->UpdatePlayerStatsUI(Value);
		}
	}
}

void UInventoryManagerComponent::RemoveFromItemAmount(FSlotStructure& InventoryItem, const uint8& AmountToRemove, bool& WasFullAmountRemoved, uint8& AmountRemoved)
{
	if (AmountToRemove >= InventoryItem.Amount)
	{
		AmountRemoved = InventoryItem.Amount;
		WasFullAmountRemoved = true;
	}
	else
	{
		const uint8 UpdatedAmount = InventoryItem.Amount - AmountToRemove;
		InventoryItem.Amount = UpdatedAmount;
		WasFullAmountRemoved = false;
	}
}

void UInventoryManagerComponent::EquipItem(UInventoryComponent* FromInventory, uint8 FromInventorySlot, UInventoryComponent* ToInventory, uint8 ToInventorySlot)
{
	if (FromInventory == ToInventory && FromInventorySlot == ToInventorySlot)
	{
		return;
	}
	if (GetItemTypeBySlot(FromInventorySlot) == EItemType::Equipment)
	{
		FSlotStructure LocalInventoryItem = FromInventory->GetInventorySlot(FromInventorySlot);
		EEquipmentSlot LocalEquipmentSlotType = GetItemEquipmentSlot(LocalInventoryItem);

		if (GetEquipmentTypeBySlot(ToInventorySlot) == LocalEquipmentSlotType)
		{
			FSlotStructure LocalSwapInventoryItem = ToInventory->GetInventorySlot(ToInventorySlot);
			if (ItemIsValid(LocalSwapInventoryItem))
			{
				if (!CanContainerStoreItems(FromInventory))
				{
					UE_LOG(LogTemp, Warning, TEXT("CONTAINER CANNOT STORE ITEMS"))
						return;
				}

				AddItem(ToInventory, ToInventorySlot, LocalInventoryItem);
				AddItem(FromInventory, FromInventorySlot, LocalSwapInventoryItem);
			}
			else
			{
				AddItem(ToInventory, ToInventorySlot, LocalInventoryItem);
				RemoveItem(FromInventory, FromInventorySlot);
			}

			if (LocalInventoryItem.ItemStructure.ID == (FName("ID_Pistol")))
			{
				if (UFunction* TriggerFunction = playerReference->FindFunction(TEXT("MocapSelectPistol")))
				{
					uint8* ParamsBuffer = static_cast<uint8*>(FMemory_Alloca(TriggerFunction->ParmsSize));
					FMemory::Memzero(ParamsBuffer, TriggerFunction->ParmsSize);
					playerReference->ProcessEvent(TriggerFunction, ParamsBuffer);
				}
// 				playerReference->UsePistol();
			}
			if (LocalInventoryItem.ItemStructure.ID == (FName("ID_Rifle")))
			{
				if (UFunction* TriggerFunction = playerReference->FindFunction(TEXT("MocapSelectRifle")))
				{
					uint8* ParamsBuffer = static_cast<uint8*>(FMemory_Alloca(TriggerFunction->ParmsSize));
					FMemory::Memzero(ParamsBuffer, TriggerFunction->ParmsSize);
					playerReference->ProcessEvent(TriggerFunction, ParamsBuffer);
				}
// 				playerReference->UseRifle();
			}
			if (LocalInventoryItem.ItemStructure.ID == (FName("ID_Knife")))
			{
				if (UFunction* TriggerFunction = playerReference->FindFunction(TEXT("MocapSelectOneHandedAxe")))
				{
					uint8* ParamsBuffer = static_cast<uint8*>(FMemory_Alloca(TriggerFunction->ParmsSize));
					FMemory::Memzero(ParamsBuffer, TriggerFunction->ParmsSize);
					playerReference->ProcessEvent(TriggerFunction, ParamsBuffer);
				}
// 				playerReference->UseKnife();
			}
			if (LocalInventoryItem.ItemStructure.ID == (FName("ID_Axe")))
			{
				if (UFunction* TriggerFunction = playerReference->FindFunction(TEXT("MocapSelectTwoHandedAxe")))
				{
					uint8* ParamsBuffer = static_cast<uint8*>(FMemory_Alloca(TriggerFunction->ParmsSize));
					FMemory::Memzero(ParamsBuffer, TriggerFunction->ParmsSize);
					playerReference->ProcessEvent(TriggerFunction, ParamsBuffer);
				}
// 				playerReference->UseAxe();
			}


			UpdateEquippedStats();
			Server_UpdateTooltips();
			return;

		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("ITEM CAN NOT BE EQUIPPED IN THAT SLOT"))
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ITEM IS NOT EQUIPPABLE"))
	}
}

void UInventoryManagerComponent::UnEquipItem(UInventoryComponent* FromInventory, uint8 FromInventorySlot, UInventoryComponent* ToInventory, uint8 ToInventorySlot)
{
	if (FromInventory == ToInventory && FromInventorySlot == ToInventorySlot)
	{
		return;
	}

	FSlotStructure LocalInventoryItem = FromInventory->GetInventorySlot(FromInventorySlot);
	FSlotStructure LocalSwapInventoryItem = ToInventory->GetInventorySlot(ToInventorySlot);
	EEquipmentSlot LocalEquipmentSlot = GetEquipmentTypeBySlot(FromInventorySlot);

	if (ItemIsValid(LocalSwapInventoryItem))
	{
		if (!CanContainerStoreItems(ToInventory))
		{
			UE_LOG(LogTemp, Warning, TEXT("CONTAINER CANNOT STORE ITEMS"))
				return;
		}

		if (GetItemTypeBySlot(FromInventorySlot) != EItemType::Equipment)
		{
			UE_LOG(LogTemp, Warning, TEXT("ITEM IS NOT EQUIPPABLE"))
				return;
		}

		if (GetEquipmentTypeBySlot(ToInventorySlot) != LocalEquipmentSlot)
		{
			UE_LOG(LogTemp, Warning, TEXT("ITEM CAN NOT BE EQUIPPED IN THAT SLOT"))
				return;
		}

		AddItem(ToInventory, ToInventorySlot, LocalInventoryItem);
		AddItem(FromInventory, FromInventorySlot, LocalSwapInventoryItem);
	}
	else
	{
		if (Cast<UEquipmentComponent>(ToInventory))
		{
			if (ToInventorySlot < (uint8)EEquipmentSlot::Count)
			{
				if (GetEquipmentTypeBySlot(ToInventorySlot) != LocalEquipmentSlot)
				{
					UE_LOG(LogTemp, Warning, TEXT("ITEM CAN NOT BE EQUIPPED IN THAT SLOT"))
						return;
				}
			}
		}

		AddItem(ToInventory, ToInventorySlot, LocalInventoryItem);
		RemoveItem(FromInventory, FromInventorySlot);
	}

	if (LocalInventoryItem.ItemStructure.ID == (FName("ID_Pistol")))
	{
		if (UFunction* TriggerFunction = playerReference->FindFunction(TEXT("MocapSelectPistol")))
		{
			uint8* ParamsBuffer = static_cast<uint8*>(FMemory_Alloca(TriggerFunction->ParmsSize));
			FMemory::Memzero(ParamsBuffer, TriggerFunction->ParmsSize);
			playerReference->ProcessEvent(TriggerFunction, ParamsBuffer);
		}
		// 				playerReference->UsePistol();
	}
	if (LocalInventoryItem.ItemStructure.ID == (FName("ID_Rifle")))
	{
		if (UFunction* TriggerFunction = playerReference->FindFunction(TEXT("MocapSelectRifle")))
		{
			uint8* ParamsBuffer = static_cast<uint8*>(FMemory_Alloca(TriggerFunction->ParmsSize));
			FMemory::Memzero(ParamsBuffer, TriggerFunction->ParmsSize);
			playerReference->ProcessEvent(TriggerFunction, ParamsBuffer);
		}
		// 				playerReference->UseRifle();
	}
	if (LocalInventoryItem.ItemStructure.ID == (FName("ID_Knife")))
	{
		if (UFunction* TriggerFunction = playerReference->FindFunction(TEXT("MocapSelectOneHandedAxe")))
		{
			uint8* ParamsBuffer = static_cast<uint8*>(FMemory_Alloca(TriggerFunction->ParmsSize));
			FMemory::Memzero(ParamsBuffer, TriggerFunction->ParmsSize);
			playerReference->ProcessEvent(TriggerFunction, ParamsBuffer);
		}
		// 				playerReference->UseKnife();
	}
	if (LocalInventoryItem.ItemStructure.ID == (FName("ID_Axe")))
	{
		if (UFunction* TriggerFunction = playerReference->FindFunction(TEXT("MocapSelectTwoHandedAxe")))
		{
			uint8* ParamsBuffer = static_cast<uint8*>(FMemory_Alloca(TriggerFunction->ParmsSize));
			FMemory::Memzero(ParamsBuffer, TriggerFunction->ParmsSize);
			playerReference->ProcessEvent(TriggerFunction, ParamsBuffer);
		}
		// 				playerReference->UseAxe();
	}
	UpdateEquippedStats();
	Server_UpdateTooltips();


}

void UInventoryManagerComponent::RandomizeDropLocation(FSlotStructure LocalSlot, UClass*& LocalClass, FTransform& OutTransform)
{
	LocalClass = LocalSlot.ItemStructure.Class;

	FVector LocalLocation{ 0.0f };
	FVector PawnLocation = Cast<AController>(GetOwner())->GetPawn()->GetActorLocation();
	const uint8 DropDistanceRangeX = FMath::RandRange(75, 100);
	FVector DistanceFromPawn{ (float)DropDistanceRangeX,1.0f,1.0f };

	const float DropItemsRotation = FMath::FRandRange(-180.0, 180.0);
	FRotator Rotation{ 1.0f, DropItemsRotation, DropItemsRotation };
	FVector VectorRotated = Rotation.RotateVector(DistanceFromPawn);
	FVector FinalLocation = PawnLocation + LocalLocation + VectorRotated;
	FRotator FinalRotator{ 1.0f, 1.0f, 1.0f };
	FVector FinalScale{ 1.0f,1.0f,1.0f };

	OutTransform = {};
	OutTransform = FTransform(FinalRotator, FinalLocation, FinalScale);
}

void UInventoryManagerComponent::DropItem(UInventoryComponent* Inventory, uint8 InventorySlot)
{
	FSlotStructure LocalSlot = Inventory->GetInventoryItem(InventorySlot);

	if (LocalSlot.ItemStructure.IsDroppable)
	{
		UE_LOG(LogTemp, Warning, TEXT("DROPPED ITEM"))

		UClass* LocalClass = nullptr;
		FTransform OutTransform;
		RandomizeDropLocation(LocalSlot, LocalClass, OutTransform);
		AWorldActor* WActor = GetWorld()->SpawnActor<AWorldActor>(LocalClass, OutTransform);
		if (WActor)
		{
			WActor->StaticMesh->SetSimulatePhysics(true);
			WActor->Amount = LocalSlot.Amount;
		}

		RemoveItem(Inventory, InventorySlot);

		if (InventorySlot < (uint8)EEquipmentSlot::Count)
		{
			UpdateEquippedStats();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("You cannot drop this..."))
			UE_LOG(LogTemp, Warning, TEXT("DESTROYED ITEM"))

			RemoveItem(Inventory, InventorySlot);
	}
}

void UInventoryManagerComponent::MoveItem(UInventoryComponent* FromInventory, uint8 FromInventorySlot, UInventoryComponent* ToInventory, uint8 ToInventorySlot)
{
	if (FromInventory == ToInventory && FromInventorySlot == ToInventorySlot)
	{
		return;
	}

	FSlotStructure LocalInventoryItem = FromInventory->GetInventoryItem(FromInventorySlot);
	FSlotStructure LocalSwapInventoryItem = ToInventory->GetInventoryItem(ToInventorySlot);

	if (!CanContainerStoreItems(ToInventory))
	{
		return;
	}

	if (LocalInventoryItem.ItemStructure.ItemType == EItemType::Currency)
	{
		AddGold(LocalInventoryItem.Amount);

		RemoveItem(FromInventory, FromInventorySlot);

		return;
	}

	if (ItemIsValid(LocalSwapInventoryItem))
	{
		if (ItemIsSame(LocalInventoryItem, LocalSwapInventoryItem) &&
			IsItemStackable(LocalSwapInventoryItem) &&
			ItemFreeStackSpace(LocalSwapInventoryItem) > 0)
		{
			uint8 AmountRemaining = 0;
			AddItemToStack(ToInventory, ToInventorySlot, LocalInventoryItem.Amount, AmountRemaining);

			if (AmountRemaining > 0)
			{

				LocalInventoryItem.Amount = AmountRemaining;

				AddItem(FromInventory, FromInventorySlot, LocalInventoryItem);

				Server_UpdateTooltips();
				return;
			}
			else
			{
				RemoveItem(FromInventory, FromInventorySlot);

				Server_UpdateTooltips();
				return;
			}
		}
		else {
			if (!CanContainerStoreItems(FromInventory))
			{
				UE_LOG(LogTemp, Warning, TEXT("CONTAINER CANNOT STORE ITEMS"))
					return;
			}
			else {
				AddItem(ToInventory, ToInventorySlot, LocalInventoryItem);
				AddItem(FromInventory, FromInventorySlot, LocalSwapInventoryItem);
			}
		}
	}
	else
	{
		AddItem(ToInventory, ToInventorySlot, LocalInventoryItem);
		RemoveItem(FromInventory, FromInventorySlot);
	}

	Server_UpdateTooltips();
}

void UInventoryManagerComponent::RemoveItem(UInventoryComponent* Inventory, uint8 InventorySlot)
{
	Inventory->ClearInventoryItem(InventorySlot);

	if (PlayerInventory == Inventory)
	{
		Client_ClearInventorySlotItem(InventorySlot);
	}
	else
	{
		ClearViewersContainerSlot(InventorySlot);
	}
}

void UInventoryManagerComponent::UseEquipmentItem(uint8 InventorySlot, FSlotStructure InventoryItem, UInventoryComponent* ToInventory)
{
	if (InventorySlot < (uint8)EEquipmentSlot::Count)
	{
		uint8 Index = 0;
		if (ToInventory == ContainerInventory)
		{
			FReturnTupleBoolInt LocalTuple = ToInventory->GetEmptyContainerSpace();
			Index = LocalTuple.Index;
			if (LocalTuple.Success)
			{
				UnEquipItem(PlayerInventory, InventorySlot, ToInventory, Index);
				return;
			}
		}
		else if (ToInventory == PlayerInventory)
		{
			if (ToInventory->GetEmptyInventorySpace(Index))
			{
				UnEquipItem(PlayerInventory, InventorySlot, ToInventory, Index);
				return;
			}
		}

		UE_LOG(LogTemp, Warning, TEXT("NO FREE SPACE"))
	}
	else
	{
		EEquipmentSlot LocalEquipmentSlot = GetItemEquipmentSlot(InventoryItem);

		uint8 OutInventorySlot = 0;
		if (GetEmptyEquipmentSlotByType(LocalEquipmentSlot, OutInventorySlot))
		{
			Server_EquipFromInventory(InventorySlot, OutInventorySlot);
		}
		else
		{
			OutInventorySlot = GetEquipmentSlotByType(LocalEquipmentSlot);
			Server_EquipFromInventory(InventorySlot, OutInventorySlot);
		}
	}
}

void UInventoryManagerComponent::UseConsumableItem(uint8 InventorySlot, FSlotStructure InventoryItem)
{
	UE_LOG(LogTemp, Warning, TEXT("Consuming this Item..."))

	// HEAL ! 
	UE_LOG(LogTemp, Warning, TEXT("Call ClientRPC_UseConsumableItem Value : %d"), InventoryItem.ItemStructure.Health);
	ClientRPC_UseConsumableItem(InventoryItem.ItemStructure.Health);

	uint8 AmountToRemove = 1;
	bool WasFullAmountRemoved = false;
	uint8 AmountRemoved = 0;

	RemoveFromItemAmount(InventoryItem, AmountToRemove, WasFullAmountRemoved, AmountRemoved);

	if (WasFullAmountRemoved)
	{
		InventoryItem = GetEmptySlot(EEquipmentSlot::Undefined);

		RemoveItem(PlayerInventory, InventorySlot);
	}
	else
	{
		AddItem(PlayerInventory, InventorySlot, InventoryItem);
	}
}
 
void UInventoryManagerComponent::ClientRPC_UseConsumableItem_Implementation(const int32 Health)
{
	GetPlayerRef()->stateComp->AddStatePoint(EStateType::HP, Health);
	UE_LOG(LogTemp, Warning, TEXT("Called ClientRPC_UseConsumableItem Value : %d"), Health);
}

void UInventoryManagerComponent::UseFurnitureItem(uint8 InventorySlot, FSlotStructure InventoryItem)
{
	UE_LOG(LogTemp, Warning, TEXT("Using Furniture Item - Slot: %d, Item ID: %s, Amount: %d"), InventorySlot, *InventoryItem.ItemStructure.ID.ToString(), InventoryItem.Amount);
	UE_LOG(LogTemp, Warning, TEXT("Item Structure Info - ID: %s, Name: %s, ItemType: %d"), *InventoryItem.ItemStructure.ID.ToString(), *InventoryItem.ItemStructure.Name.ToString(), (int32)InventoryItem.ItemStructure.ItemType);

	FName ItemID = InventoryItem.ItemStructure.ID;

	uint8 AmountToRemove = 1;
	bool WasFullAmountRemoved = false;
	uint8 AmountRemoved = 0;

	RemoveFromItemAmount(InventoryItem, AmountToRemove, WasFullAmountRemoved, AmountRemoved);

	if (WasFullAmountRemoved)
	{
		InventoryItem = GetEmptySlot(EEquipmentSlot::Undefined);

		RemoveItem(PlayerInventory, InventorySlot);
	}
	else
	{
		AddItem(PlayerInventory, InventorySlot, InventoryItem);
	}
	

	ClientRPC_UseFurnitureItem(ItemID);



	//========================================================================================================
	// 인벤토리 아이템의 RowName을 가져와서 BuildID를 설정
	//FName name = InventoryItem.ItemStructure.ID;
	//UE_LOG(LogTemp, Warning, TEXT("TESTUsing Furniture Item with ID: %s"), *name.ToString());
	//TArray<FName> names = playerReference->HousingComponent->RowNames;
	//UE_LOG(LogTemp, Warning, TEXT("TESTNumber of RowNames: %d"), names.Num());
	//for (int i = 0; i < names.Num(); i++)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("TESTComparing with RowName: %s"), *names[i].ToString());
	//	if (name.ToString() == names[i].ToString())
	//	{
	//		housingComponent->BuildID = i;
	//		UE_LOG(LogTemp, Warning, TEXT("TESTBuildID set to: %d"), i);
	//		playerReference->HousingComponent->LaunchBuildMode();
	//		break;
	//	}
	//}
	// if (playerReference && playerReference->HousingComponent)
	//playerReference->HousingComponent->LaunchBuildMode();
	//for (int32 i = 0; i < housingComponent->Buildables.Num(); ++i)
	//{
	//	if (housingComponent->Buildables[i].ID == ItemRowName)
	//	{
	//		housingComponent->BuildID = i;
	//		break;
	//	}
	//}
	//housingComponent->CurrentBuildID = ItemID;



	// 인벤토리에서 선택한 아이템에 따라 BuildID 설정 ?
	//playerReference->HousingComponent->BuildID = InventoryItem.ItemStructure.ID;

	//playerReference->HousingComponent->LaunchBuildMode();
}

void UInventoryManagerComponent::ClientRPC_UseFurnitureItem_Implementation(FName ItemID)
{

	// 인벤토리, 장비창, 보관함 UI 켜져있으면 꺼버리기 
	if (GetPlayerHud()->HUDReference->MainLayout->Inventory->IsVisible())
	{
		GetPlayerHud()->HUDReference->MainLayout->Inventory->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (GetPlayerHud()->HUDReference->MainLayout->Profile->IsVisible())
	{
		GetPlayerHud()->HUDReference->MainLayout->Profile->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (GetPlayerHud()->HUDReference->MainLayout->Container->IsVisible())
	{
		GetPlayerHud()->HUDReference->MainLayout->Container->SetVisibility(ESlateVisibility::Collapsed);
	}

	// 	playerReference->HousingComponent->LaunchBuildMode();
	if (GetPlayerRef() && GetPlayerRef()->HousingComponent)
	{

		housingComponent = GetPlayerRef()->HousingComponent;

		bool bBuildIDSet = false;
		for (int32 i = 0; i < housingComponent->Buildables.Num(); i++)
		{
			FName BuildableID = housingComponent->Buildables[i].ID;
			UE_LOG(LogTemp, Warning, TEXT("Comparing Item ID: %s with Buildable ID: %s"), *ItemID.ToString(), *BuildableID.ToString());

			if (BuildableID == ItemID)
			{
				housingComponent->BuildID = i;
				bBuildIDSet = true;
				UE_LOG(LogTemp, Warning, TEXT("Matching BuildID found: %d for Item ID: %s"), i, *ItemID.ToString());
				break;
			}
		}

		if (bBuildIDSet)
		{
			GetPlayerRef()->HousingComponent->IsBuildModeOn = false;
			GetPlayerRef()->HousingComponent->LaunchBuildMode(ItemID);

		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No matching BuildID found in housing data table for Item ID: %s"), *ItemID.ToString());
		}
	}
}

 void UInventoryManagerComponent::UseInventoryItem(const uint8& InventorySlot)
{
	FSlotStructure LocalInventorySlot = PlayerInventory->GetInventoryItem(InventorySlot);

	if (IsValid(CurrentContainer))
	{
		bool bCanStoreItems = IInventoryInterface::Execute_GetCanStoreItems(CurrentContainer);

		if (bCanStoreItems)
		{

			switch (LocalInventorySlot.ItemStructure.ItemType)
			{
			case EItemType::Equipment:
				if (InventorySlot < (uint8)EEquipmentSlot::Count)
				{
					UseEquipmentItem(InventorySlot, LocalInventorySlot, ContainerInventory);
					break;
				}
			case EItemType::Furniture:
			case EItemType::Consumable:
			case EItemType::Currency:
			default:
				bool bOutSuccess = false;
				TryToAddItemToInventory(ContainerInventory, LocalInventorySlot, bOutSuccess);

				if (bOutSuccess)
				{
					RemoveItem(PlayerInventory, InventorySlot);
				}
				else
				{
					AddItem(PlayerInventory, InventorySlot, LocalInventorySlot);
				}
				break;
			}

			return;
		}
	}
	switch (LocalInventorySlot.ItemStructure.ItemType)
	{
	case EItemType::Furniture:
		UseFurnitureItem(InventorySlot, LocalInventorySlot);
		break;
	case EItemType::Consumable:
		UseConsumableItem(InventorySlot, LocalInventorySlot);
		break;
	case EItemType::Equipment:
		UseEquipmentItem(InventorySlot, LocalInventorySlot, PlayerInventory);
		break;
	default:
		break;
	}
	Server_UpdateTooltips();
	Client_CheckHotbarSlots(LocalInventorySlot);
}

void UInventoryManagerComponent::UseContainerItem(const uint8& InventorySlot)
{
	FSlotStructure LocalInventoryItem = ContainerInventory->GetInventoryItem(InventorySlot);

	bool bOutSuccess = false;
	TryToAddItemToInventory(PlayerInventory, LocalInventoryItem, bOutSuccess);

	if (bOutSuccess)
	{
		RemoveItem(ContainerInventory, InventorySlot);
	}
	else
	{
		AddItem(ContainerInventory, InventorySlot, LocalInventoryItem);
	}

	Server_UpdateTooltips();

	FSlotStructure LocalSlot{};
	bool IsThereMoreItems = false;
	for (uint8 i = 0; i < ContainerInventory->Inventory.Num(); i++)
	{
		LocalSlot = ContainerInventory->Inventory[i];

		if (LocalSlot.Amount > 0)
		{
			IsThereMoreItems = true;
			break;
		}
	}

	if (!IsThereMoreItems)
	{
		if (Cast<ALootActor>(CurrentContainer))
		{
			IInventoryInterface::Execute_ContainerLooted(CurrentContainer);
		}
	}
}

void UInventoryManagerComponent::ClearInventoryItem(const uint8& InventorySlot)
{
	PlayerInventory->Inventory[InventorySlot] = GetEmptySlot(GetEquipmentTypeBySlot(InventorySlot));
}

void UInventoryManagerComponent::SetInventorySlotItem(const FSlotStructure& ContentToAdd, const uint8& InventorySlot)
{
	if (IsValid(MainLayoutUI))
	{
		USlotLayout* SlotLayout{};
		if (InventorySlot >= (uint8)EEquipmentSlot::Count)
		{
			uint8 LocalIndex = InventorySlot - (uint8)EEquipmentSlot::Count;
			SlotLayout = MainLayoutUI->Inventory->InventorySlotsArray[LocalIndex];
		}
		else
		{
			SlotLayout = MainLayoutUI->Profile->EquipmentSlotsArray[InventorySlot];
		}

		SlotLayout->UpdateSlot(ContentToAdd);
	}
}

void UInventoryManagerComponent::ClearInventorySlotItem(uint8 InventorySlot)
{
	if (IsValid(MainLayoutUI))
	{
		USlotLayout* SlotLayout{};
		FSlotStructure LocalSlot{};
		if (InventorySlot >= (uint8)EEquipmentSlot::Count)
		{
			uint8 LocalIndex = InventorySlot - (uint8)EEquipmentSlot::Count;

			SlotLayout = MainLayoutUI->Inventory->InventorySlotsArray[LocalIndex];
			LocalSlot = GetEmptySlot(EEquipmentSlot::Undefined);
		}
		else
		{
			SlotLayout = MainLayoutUI->Profile->EquipmentSlotsArray[InventorySlot];

			switch (InventorySlot)
			{
			case 0: LocalSlot = GetEmptySlot(EEquipmentSlot::Weapon);
				break;
			case 1: LocalSlot = GetEmptySlot(EEquipmentSlot::Chest);
				break;
			case 2: LocalSlot = GetEmptySlot(EEquipmentSlot::Feet);
				break;
			case 3: LocalSlot = GetEmptySlot(EEquipmentSlot::Hands);
				break;
			case 4: LocalSlot = GetEmptySlot(EEquipmentSlot::Legs);
				break;
			case 5: LocalSlot = GetEmptySlot(EEquipmentSlot::Head);
				break;
			default: LocalSlot = GetEmptySlot(EEquipmentSlot::Undefined);
				break;
			}
		}

		SlotLayout->UpdateSlot(LocalSlot);

		MainLayoutUI->Inventory->UpdateGoldAmount();
	}
}

FSlotStructure UInventoryManagerComponent::GetInventorySlotItem(uint8 InventorySlot)
{
	uint8 Index = InventorySlot;
	if (InventorySlot >= (uint8)EEquipmentSlot::Count)
	{
		Index = InventorySlot - (uint8)EEquipmentSlot::Count;
	}

	FSlotStructure Slot = MainLayoutUI->Inventory->InventorySlotsArray[Index]->SlotStructure;
	return Slot;
}

void UInventoryManagerComponent::AddGold(uint8 Amount)
{
	Gold += Amount;
	OnRep_UpdateGoldAmount();
}
void UInventoryManagerComponent::ClearContainerSlots()
{
	MainLayoutUI->Container->ContainerSlotsArray.Empty();
	MainLayoutUI->Container->ContainerGridPanel->ClearChildren();
}

void UInventoryManagerComponent::CreateContainerSlots(uint8 NumberOfRows, uint8 SlotsPerRow)
{
	ClearContainerSlots();

	AVICTIMSPlayerController* PC = Cast<AVICTIMSPlayerController>(GetOwner());

	FWidgetsLayoutBP* WidgetLayout = Cast<AMyHUD>(PC->HUD_Reference)->GetWidgetBPClass("SlotLayout_WBP");
	if (WidgetLayout)
	{
		USlotLayout* W_ContainerSlot = nullptr;

		for (int i = 0; i < NumberOfRows; i++)
		{
			for (int j = 0; j < SlotsPerRow; j++)
			{
				W_ContainerSlot = CreateWidget<USlotLayout>(GetWorld(), WidgetLayout->Widget);
				MainLayoutUI->Container->ContainerGridPanel->AddChildToUniformGrid(W_ContainerSlot, i, j);

				MainLayoutUI->Container->ContainerSlotsArray.Add(W_ContainerSlot);
			}
		}

		FSlotStructure SlotStructure = GetEmptySlot(EEquipmentSlot::Undefined);

		for (int i = 0; i < MainLayoutUI->Container->ContainerSlotsArray.Num(); i++)
		{
			MainLayoutUI->Container->ContainerSlotsArray[i]->UpdateSlot(SlotStructure);
			MainLayoutUI->Container->ContainerSlotsArray[i]->InventorySlotIndex = i;
			MainLayoutUI->Container->ContainerSlotsArray[i]->NativeFromContainer = true;
			MainLayoutUI->Container->ContainerSlotsArray[i]->IsStorageSlot = true;
		}
	}
}

void UInventoryManagerComponent::SetViewersContainerSlot(uint8 ContainerSlot, FSlotStructure& InventoryItem)
{
	TArray<APlayerState*> TempPlayersViewing = IInventoryInterface::Execute_GetPlayersViewing(CurrentContainer);

	for (APlayerState* PlayerState : TempPlayersViewing)
	{
		if (AVICTIMSPlayerController* PC = Cast<AVICTIMSPlayerController>(PlayerState->GetOwner()))
		{
			PC->InventoryManagerComponent->Client_SetContainerSlotItem(InventoryItem, ContainerSlot);
		}
	}
}

void UInventoryManagerComponent::CreateContainerSlots2(uint8 InventorySize, uint8 SlotsPerRow)
{
	ClearContainerSlots();

	if (InventorySize <= 0)
	{
		return;
	}

	bool bLocalIsStorageContainer = MainLayoutUI->Container->IsStorageContainer;
	uint8 LocalContainerSize = InventorySize;
	uint8 LocalLoopCount = 0;

	for (uint8 i = 0; i < LocalContainerSize; i++)
	{
		AddContainerSlot(i, 0, LocalLoopCount, bLocalIsStorageContainer);
		LocalLoopCount++;

		if (LocalLoopCount == LocalContainerSize)
		{
			break;
		}
	}
}

void UInventoryManagerComponent::AddContainerSlot(uint8 Row, uint8 Column, uint8 Slot, bool IsStorage)
{
	AVICTIMSPlayerController* PC = Cast<AVICTIMSPlayerController>(GetOwner());

	FWidgetsLayoutBP* WidgetLayout = Cast<AMyHUD>(PC->HUD_Reference)->GetWidgetBPClass("SlotLayout_WBP");
	USlotLayout* LocalSlot = CreateWidget<USlotLayout>(GetWorld(), WidgetLayout->Widget);

	MainLayoutUI->Container->ContainerGridPanel->AddChildToUniformGrid(LocalSlot, Row, Column);

	MainLayoutUI->Container->ContainerSlotsArray.Add(LocalSlot);

	LocalSlot->IsStorageSlot = IsStorage;
	LocalSlot->InventorySlotIndex = Slot;
	LocalSlot->NativeFromContainer = true;

	LocalSlot->SetNameBoxVisibility();
}

void UInventoryManagerComponent::ClearContainerSlotItem(uint8 ContainerSlot)
{
	USlotLayout* SlotLayout = MainLayoutUI->Container->ContainerSlotsArray[ContainerSlot];
	SlotLayout->UpdateSlot(GetEmptySlot(EEquipmentSlot::Undefined));
}

void UInventoryManagerComponent::ClearViewersContainerSlot(uint8 ContainerSlot)
{
	TArray<APlayerState*> LocalPlayersViewing = IInventoryInterface::Execute_GetPlayersViewing(CurrentContainer);

	for (APlayerState* PlayerState : LocalPlayersViewing)
	{
		if (AVICTIMSPlayerController* PlayerController = Cast<AVICTIMSPlayerController>(PlayerState->GetOwner()))
		{
			PlayerController->InventoryManagerComponent->Client_ClearContainerSlotItem(ContainerSlot);
		}
	}
}

void UInventoryManagerComponent::SetContainerSlotItem(const FSlotStructure& Slot, uint8 Index)
{
	USlotLayout* SlotLayout = MainLayoutUI->Container->ContainerSlotsArray[Index];
	SlotLayout->UpdateSlot(Slot);
}

void UInventoryManagerComponent::OpenContainer(AActor* Container)
{
	CurrentContainer = Container;

	UInventoryComponent* GetContainerInventoryTemp{};
	IInventoryInterface::Execute_GetContainerInventory(CurrentContainer, GetContainerInventoryTemp);
	ContainerInventory = GetContainerInventoryTemp;

	TArray<FSlotStructure> LocalInventory{};
	FSlotStructure LocalEmptySlot = GetEmptySlot(EEquipmentSlot::Undefined);
	for (FSlotStructure Slot : ContainerInventory->Inventory)
	{
		if (Slot.Amount == 0)
		{
			LocalInventory.Add(LocalEmptySlot);
		}
		else
		{
			LocalInventory.Add(Slot);
		}
	}

	FName LocalName;
	uint8 LocalNumberOfRows;
	uint8 LocalSlotsPerRow;
	bool LocalIsStorageContainer;
	uint8 LocalInventorySize;

	IInventoryInterface::Execute_GetContainerProperties(Container, LocalName, LocalNumberOfRows, LocalSlotsPerRow,
		LocalIsStorageContainer, LocalInventorySize);
	FContainerInfo C_Info;
	C_Info.ContainerName = LocalName;
	C_Info.NumberOfRows = LocalNumberOfRows;
	C_Info.SlotsPerRow = LocalSlotsPerRow;
	C_Info.IsStorageContainer = LocalIsStorageContainer;
	C_Info.StorageInventorySize = LocalInventorySize;

	Client_OpenContainer(C_Info, LocalInventory, PlayerInventory->Inventory);
}

void UInventoryManagerComponent::UseContainer(AActor* Container)
{
	if (Container->Implements<UInventoryInterface>())
	{
		if (CurrentContainer != Container)
		{
			OpenContainer(Container);
		}
		else
		{
			Server_CloseContainer();
		}
	}
}

void UInventoryManagerComponent::CloseContainer()
{
	if (AVICTIMSPlayerController* PlayerController = Cast<AVICTIMSPlayerController>(GetOwner()))
	{
		if (AContainerActor* CurrentContainerActor = Cast<AContainerActor>(CurrentContainer))
		{
			CurrentContainerActor->PlayersViewing.Remove(PlayerController->PlayerState);
		}
	}

	CurrentContainer = nullptr;
	ContainerInventory = nullptr;

	Client_CloseContainer();
}

void UInventoryManagerComponent::LoadContainerSlots(FContainerInfo ContainerProperties, const TArray<FSlotStructure>& InContainerInventory, const TArray<FSlotStructure>& InPlayerInventory)
{
	if (!ContainerProperties.IsStorageContainer)
	{
		MainLayoutUI->Container->IsStorageContainer = false;

		CreateContainerSlots2(InContainerInventory.Num(), ContainerProperties.SlotsPerRow);
	}
	else {
		MainLayoutUI->Container->IsStorageContainer = true;

		CreateContainerSlots(ContainerProperties.NumberOfRows, ContainerProperties.SlotsPerRow);
	}

	uint8 Index = 0;
	for (FSlotStructure Slot : InContainerInventory)
	{
		SetContainerSlotItem(Slot, Index);

		Index++;
	}

	Client_UpdateContainerTooltips(InPlayerInventory, InContainerInventory);

	if (AVICTIMSPlayerController* PC = Cast<AVICTIMSPlayerController>(GetOwner()))
	{
		PC->ToggleContainer();
	}

}

bool UInventoryManagerComponent::CanContainerStoreItems(UInventoryComponent* Inventory)
{
	if (IsValid(CurrentContainer))
	{
		UInventoryComponent* LocalInventory{};

		IInventoryInterface::Execute_GetContainerInventory(CurrentContainer, LocalInventory);

		if (Inventory == LocalInventory)
		{
			bool LocalCanStoreItems = IInventoryInterface::Execute_GetCanStoreItems(CurrentContainer);

			if (!LocalCanStoreItems)
			{
				return false;
			}
		}
	}

	return true;
}

void UInventoryManagerComponent::FindAndAddAmountToStacks(UInventoryComponent* Inventory, FName ItemID, uint8 Amount, uint8& AmountRemaining)
{
	uint8 LocalAmount = Amount;
	FName LocalItemID = ItemID;
	UInventoryComponent* LocalInventory = Inventory;

	uint8 LocalInventorySlot = 0;
	uint8 InitialIndex = 0;
	if (LocalInventory == PlayerInventory)
	{
		InitialIndex = (uint8)EEquipmentSlot::Count;
		LocalInventorySlot = (uint8)EEquipmentSlot::Count;
	}

	for (uint8 i = InitialIndex; i < LocalInventory->Inventory.Num(); i++)
	{
		if (LocalItemID == LocalInventory->Inventory[LocalInventorySlot].ItemStructure.ID)
		{
			AddItemToStack(LocalInventory, LocalInventorySlot, LocalAmount, AmountRemaining);
			LocalAmount = AmountRemaining;

			if (LocalAmount == 0)
			{
				break;
			}
		}
		LocalInventorySlot++;
	}

	AmountRemaining = LocalAmount;
}

void UInventoryManagerComponent::AddItemToStack(UInventoryComponent* Inventory, uint8 InventorySlot, uint8 AmountToAdd, uint8& AmountRemaining)
{
	uint8 LocalRemainingAmount = AmountToAdd;

	FSlotStructure LocalInventoryItem = Inventory->GetInventoryItem(InventorySlot);

	// Does Stack Has Free Space?
	if (GetItemAmount(LocalInventoryItem) < GetItemMaxStackSize(LocalInventoryItem))
	{
		// Does The Full Amount To Add Fit In Stack?
		if (AmountToAdd <= ItemFreeStackSpace(LocalInventoryItem))
		{
			LocalRemainingAmount = 0;

			AddAmountToItem(LocalInventoryItem, AmountToAdd);
			AddItem(Inventory, InventorySlot, LocalInventoryItem);
		}
		else
		{
			LocalRemainingAmount = AmountToAdd - ItemFreeStackSpace(LocalInventoryItem);
			LocalInventoryItem.Amount = GetItemMaxStackSize(LocalInventoryItem);
			AddItem(Inventory, InventorySlot, LocalInventoryItem);
		}
	}

	AmountRemaining = LocalRemainingAmount;
}

AVICTIMSCharacter* UInventoryManagerComponent::GetPlayerRef()
{

	if (playerReference == nullptr)
	{
		auto ownerCheck = GetOwner();
		if (ownerCheck)
		{

			auto conCheck = Cast<AVICTIMSPlayerController>(ownerCheck);
			if (conCheck)
			{
				auto charCheck = Cast<AVICTIMSCharacter>(conCheck->GetPawn());
				if (charCheck)
				{
					playerReference = charCheck;
				}
			}
		}
	}

	return playerReference;
}

AMyHUD* UInventoryManagerComponent::GetPlayerHud()
{
	if (HUD == nullptr)
	{
		auto ownerCheck = GetOwner();
		if (ownerCheck)
		{
			auto conCheck = Cast<AVICTIMSPlayerController>(ownerCheck);
			if (conCheck)
			{
				HUD = Cast<AMyHUD>(conCheck->GetHUD());

				//HUD = conCheck->GetHUD();

				int iTemp = 0;
			}
		}
	}

	return HUD;
}

void UInventoryManagerComponent::Server_UseShop_Implementation(AActor* Shop)
{
	UseShop(Shop);
	Client_UpdateShopTooltips(PlayerInventory->Inventory, ShopInventory->Inventory);
}

void UInventoryManagerComponent::Server_CloseShop_Implementation()
{
	CloseShop();
}

void UInventoryManagerComponent::Server_PerchaseShopItem_Implementation(const uint8& InventorySlot)
{
	PerchaseShopItem(InventorySlot);
}

void UInventoryManagerComponent::Server_SellItem_Implementation(const uint8& InventorySlot)
{
	SellItem(InventorySlot);
}

void UInventoryManagerComponent::Client_SetShopSlotItem_Implementation(const FSlotStructure& ContentToAdd, const uint8& InventorySlot)
{
	SetShopSlotItem(ContentToAdd, InventorySlot);
}

void UInventoryManagerComponent::Client_ClearShopSlotItem_Implementation(uint8 ShopSlot)
{
	ClearShopSlotItem(ShopSlot);
}

void UInventoryManagerComponent::Client_OpenShop_Implementation(FShopInfo ShopProperties, const TArray<FSlotStructure>& InShopInventory, const TArray<FSlotStructure>& InPlayerInventory)
{
	LoadShopSlots(ShopProperties, InShopInventory, InPlayerInventory);
}

void UInventoryManagerComponent::Client_CloseShop_Implementation()
{
	MainLayoutUI->Shop->SetVisibility(ESlateVisibility::Hidden);
}

void UInventoryManagerComponent::Client_UpdateShopTooltips_Implementation(const TArray<FSlotStructure>& InPlayerInventory, const TArray<FSlotStructure>& InOtherInventory)
{
	AVICTIMSPlayerController* PC = Cast<AVICTIMSPlayerController>(GetOwner());
	if (!IsValid(PC))
	{
		return;
	}
	FWidgetsLayoutBP* WidgetLayout = Cast<AMyHUD>(PC->HUD_Reference)->GetWidgetBPClass("ItemTooltip_WBP");
	UW_ItemTooltip* Tooltip{};
	FSlotStructure TempSlot{};
	UShop_Slo* SlotLayout{};

	uint8 Index = 0;
	uint8 LocalIndex = 0;

	for (FSlotStructure Slot : InOtherInventory)
	{
		TempSlot = GetEmptySlot(EEquipmentSlot::Undefined);
		SlotLayout = MainLayoutUI->Shop->ShopSlotsArray[Index];
		Tooltip = Cast<UW_ItemTooltip>(SlotLayout->GetToolTip());
		if (!IsValid(Tooltip))
		{
			Tooltip = CreateWidget<UW_ItemTooltip>(GetWorld(), WidgetLayout->Widget);
			UE_LOG(LogTemp, Verbose, TEXT("Creating Inventory Tooltip"))
		}
		if (Slot.ItemStructure.ItemType == EItemType::Equipment)
		{
			for (uint8 j = 0; j < (uint8)EEquipmentSlot::Count; j++)
			{
				if (GetItemEquipmentSlot(InPlayerInventory[j]) == GetItemEquipmentSlot(Slot))
				{
					TempSlot = InPlayerInventory[j];
				}
			}
		}
		Tooltip->UpdateTooltipAttributes(Slot.ItemStructure, TempSlot);
		SlotLayout->ToggleTooltip();
		SlotLayout->SetToolTip(Tooltip);

		Index++;
	}
}

void UInventoryManagerComponent::PerchaseShopItem(const uint8& InventorySlot)
{
	FSlotStructure LocalInventoryItem = ShopInventory->GetInventoryItem(InventorySlot);
	AVICTIMSPlayerController* PC = Cast<AVICTIMSPlayerController>(GetOwner());
	if (LocalInventoryItem.ItemStructure.PriceValue > PC->UIGetPlayerGold())
	{	// 선택한 아이템을 살 돈이 충분하지 않을 때 
//		UE_LOG(LogTemp, Warning, TEXT("Don't have enough money"));
		MainLayoutUI->Shop->ShopSlotsArray[InventorySlot]->ShowNotification();
		return;
	}
	bool bOutSuccess = false;
	TryToAddItemToInventory(PlayerInventory, LocalInventoryItem, bOutSuccess);	// 플레이어 인벤토리에 아이템 추가
	if (bOutSuccess)
	{
// 		UE_LOG(LogTemp, Warning, TEXT("Successfully Perchased!"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Perchase Failed"));
	}
	Server_UpdateTooltips();
	AddGold(-LocalInventoryItem.ItemStructure.PriceValue);	//AddGold 사용 -> Amount 를 음수값으로 넣어줌 
}

void UInventoryManagerComponent::SellItem(const uint8& InventorySlot)
{
	FSlotStructure LocalInventorySlot = PlayerInventory->GetInventoryItem(InventorySlot);
	int32 TotalPriceValue = LocalInventorySlot.ItemStructure.PriceValue * LocalInventorySlot.Amount;
	if (IsValid(CurrentShop))
	{
		bool bCanShopItems = IInventoryInterface::Execute_GetCanShopItems(CurrentShop);
		if (bCanShopItems)
		{
			AddGold(TotalPriceValue);	// 아이템 개수 * 개당 판매가 만큼 플레이어 골드 추가
			RemoveItem(PlayerInventory, InventorySlot);
		}
	}
	Server_UpdateTooltips();
}

void UInventoryManagerComponent::ClearShopSlots()
{
	MainLayoutUI->Shop->ShopSlotsArray.Empty();
	MainLayoutUI->Shop->ShopGirdPanel->ClearChildren();
}

void UInventoryManagerComponent::CreateShopSlots(uint8 NumberOfRows, uint8 SlotsPerRow)
{
	ClearShopSlots();
	AVICTIMSPlayerController* PC = Cast<AVICTIMSPlayerController>(GetOwner());
	FWidgetsLayoutBP* WidgetLayout = Cast<AMyHUD>(PC->HUD_Reference)->GetWidgetBPClass("ShopSlot_WBP");
	if (WidgetLayout)
	{
		UShop_Slo* W_ShopSlot = nullptr;
		for (int i = 0; i < NumberOfRows; i++)
		{
			W_ShopSlot = CreateWidget<UShop_Slo>(GetWorld(), WidgetLayout->Widget);
			MainLayoutUI->Shop->ShopGirdPanel->AddChild(W_ShopSlot);
			MainLayoutUI->Shop->ShopSlotsArray.Add(W_ShopSlot);
		}
	}
	FSlotStructure SlotStructure = GetEmptySlot(EEquipmentSlot::Undefined);
	for (int i = 0; i < MainLayoutUI->Shop->ShopSlotsArray.Num(); i++)
	{
		MainLayoutUI->Shop->ShopSlotsArray[i]->UpdateSlot(SlotStructure);
		MainLayoutUI->Shop->ShopSlotsArray[i]->InventorySlotIndex = i;
		MainLayoutUI->Shop->ShopSlotsArray[i]->NativeFromShop = true;
	}
}

void UInventoryManagerComponent::CreateShopSlots2(uint8 InventorySIze, uint8 SlotsPerRow)
{
	ClearContainerSlots();
	if (InventorySIze <= 0)
	{
		return;
	}
	bool bLocalIsShop = MainLayoutUI->Shop->IsShop;
	uint8 LocalShopSize = InventorySIze;
	uint8 LocalLoopCount = 0;
	for (uint8 i = 0; i < LocalShopSize; i++)
	{
		AddShopSlot(i, 0, LocalLoopCount, bLocalIsShop);
		LocalLoopCount++;
		if (LocalLoopCount == LocalShopSize)
		{
			break;
		}
	}
}

void UInventoryManagerComponent::SetViewersShopSlot(uint8 ShopSLot, FSlotStructure& InventoryItem)
{
	TArray<APlayerState*> TempPlayersViewing = IInventoryInterface::Execute_GetPlayersViewing(CurrentShop);
	for (APlayerState* PlayerState : TempPlayersViewing)
	{
		if (AVICTIMSPlayerController* PC = Cast<AVICTIMSPlayerController>(PlayerState->GetOwner()))
		{
			PC->InventoryManagerComponent->Client_SetShopSlotItem(InventoryItem, ShopSLot);
		}
	}
}

void UInventoryManagerComponent::AddShopSlot(uint8 Row, uint8 Column, uint8 Slot, bool IsShop)
{
	AVICTIMSPlayerController* PC = Cast<AVICTIMSPlayerController>(GetOwner());
	FWidgetsLayoutBP* WidgetLayout = Cast<AMyHUD>(PC->HUD_Reference)->GetWidgetBPClass("ShopSlot_WBP");
	UShop_Slo* LocalSlot = CreateWidget<UShop_Slo>(GetWorld(), WidgetLayout->Widget);
	
	MainLayoutUI->Shop->ShopGirdPanel->AddChild(LocalSlot);
	MainLayoutUI->Shop->ShopSlotsArray.Add(LocalSlot);

	LocalSlot->IsShopSlot = IsShop;
	LocalSlot->InventorySlotIndex = Slot;
	LocalSlot->NativeFromShop = true;
}

void UInventoryManagerComponent::ClearShopSlotItem(uint8 ShopSlot)
{
	UShop_Slo* SlotLayout = MainLayoutUI->Shop->ShopSlotsArray[ShopSlot];
	SlotLayout->UpdateSlot(GetEmptySlot(EEquipmentSlot::Undefined));
}

void UInventoryManagerComponent::ClearViewersShopSlot(uint8 ShopSlot)
{
	TArray<APlayerState*> LocalPlayersViewing = IInventoryInterface::Execute_GetPlayersViewing(CurrentShop);
	for (APlayerState* PlayerState : LocalPlayersViewing)
	{
		if (AVICTIMSPlayerController* PlayerController = Cast<AVICTIMSPlayerController>(PlayerState->GetOwner()))
		{
			PlayerController->InventoryManagerComponent->Client_ClearShopSlotItem(ShopSlot);
		}
	}
}

void UInventoryManagerComponent::SetShopSlotItem(const FSlotStructure& Slot, uint8 Index)
{
	UShop_Slo* SlotLayout = MainLayoutUI->Shop->ShopSlotsArray[Index];
	SlotLayout->UpdateSlot(Slot);
	SlotLayout->SetPriceValue(Slot.ItemStructure);
}

void UInventoryManagerComponent::OpenShop(AActor* Shop)
{
	CurrentShop = Shop;
	UInventoryComponent* GetShopInventoryTemp{};
	IInventoryInterface::Execute_GetShopInventory(CurrentShop, GetShopInventoryTemp);
	ShopInventory = GetShopInventoryTemp;

	TArray<FSlotStructure> LocalInventory{};
	FSlotStructure LocalEmptySlot = GetEmptySlot(EEquipmentSlot::Undefined);
	for (FSlotStructure Slot : ShopInventory->Inventory)
	{
		if (Slot.Amount == 0)
		{
			LocalInventory.Add(LocalEmptySlot);
		}
		else
		{
			LocalInventory.Add(Slot);
		}
	}
	FName LocalName;
	uint8 LocalNumberOfRows;
	uint8 LocalSlotsPerRow;
	bool LocalIsShop;
	uint8 LocalInventorySize;

	IInventoryInterface::Execute_GetShopProperties(Shop, LocalName, LocalNumberOfRows, LocalSlotsPerRow, LocalIsShop, LocalInventorySize);
	FShopInfo S_Info;
	S_Info.ShopName = LocalName;
	S_Info.NumberOfRows = LocalNumberOfRows;
	S_Info.SlotsPerRow = LocalSlotsPerRow;
	S_Info.IsShop = LocalIsShop;
	S_Info.ShopInventorySize = LocalInventorySize;

	Client_OpenShop(S_Info, LocalInventory, PlayerInventory->Inventory);
}

void UInventoryManagerComponent::UseShop(AActor* Shop)
{
	if (Shop->Implements<UInventoryInterface>())
	{
		if (CurrentShop != Shop)
		{
			OpenShop(Shop);
		}
		else
		{
			Server_CloseShop();
		}
	}
}

void UInventoryManagerComponent::CloseShop()
{
	if (AVICTIMSPlayerController* PlayerController = Cast<AVICTIMSPlayerController>(GetOwner()))
	{
		if (AShopActor* CurrentShopActor = Cast<AShopActor>(CurrentShop))
		{
			CurrentShopActor->PlayersViewing.Remove(PlayerController->PlayerState);
		}
	}
	CurrentShop = nullptr;
	ShopInventory = nullptr;
	Client_CloseShop();
}

void UInventoryManagerComponent::LoadShopSlots(FShopInfo ShopProperties, const TArray<FSlotStructure>& InShopInventroy, const TArray<FSlotStructure>& InPlayerInventory)
{
	if (!ShopProperties.IsShop)
	{
		MainLayoutUI->Shop->IsShop = false;
		CreateShopSlots2(InShopInventroy.Num(), ShopProperties.SlotsPerRow);
	}
	else
	{
		MainLayoutUI->Shop->IsShop = true;
		CreateShopSlots(ShopProperties.NumberOfRows, ShopProperties.SlotsPerRow);
	}
	uint8 Index = 0;
	for (FSlotStructure Slot : InShopInventroy)
	{
		SetShopSlotItem(Slot, Index);
		Index++;
	}
	Client_UpdateShopTooltips(InPlayerInventory, InShopInventroy);
	if (AVICTIMSPlayerController* PC = Cast<AVICTIMSPlayerController>(GetOwner()))
	{
		PC->ToggleShop();
	}

}

bool UInventoryManagerComponent::CanShopItems(UInventoryComponent* Inventory)
{
	if (IsValid(CurrentShop))
	{
		UInventoryComponent* LocalInventory{};
		IInventoryInterface::Execute_GetShopInventory(CurrentShop, LocalInventory);
		if (Inventory == LocalInventory)
		{
			bool LocalShopItems = IInventoryInterface::Execute_GetCanShopItems(CurrentShop);
			if (!LocalShopItems)
			{
				return false;
			}
		}
	}
	return true;
}
