
#include "ShopActor.h"
#include "AVICTIMSPlayerController.h"
#include "GameFramework/PlayerState.h"
#include "InventoryManagerComponent.h"
#include "FShopList.h"
#include "VictimsGameInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include <Net/UnrealNetwork.h>

AShopActor::AShopActor()
{
	PrimaryActorTick.bCanEverTick = false;

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory Component"));
	InventoryComponent->NumberOfRowsInventory = 20;
	InventoryComponent->SlotsPerRowInventory = 1;
	S_Name = "Shop";
	S_CanShopItems = true;

	Body = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Body"));
	Body->SetupAttachment(GetRootComponent());

	Chest = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Chest"));
	Chest->SetupAttachment(Body);

	Hands = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Hands"));
	Hands->SetupAttachment(Body);

	Feet = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Feet"));
	Feet->SetupAttachment(Body);

	Legs = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Legs"));
	Legs->SetupAttachment(Body);

	Head = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Head"));
	Head->SetupAttachment(Body);
}


void AShopActor::BeginPlay()
{
	Super::BeginPlay();		

	uint8 LocalNumberOfRows = InventoryComponent->NumberOfRowsInventory;
	uint8 LocalNumberOfSlotsPerRow = InventoryComponent->SlotsPerRowInventory;

	if (LocalNumberOfRows * LocalNumberOfSlotsPerRow < ShopItemAmount)
	{
		for (uint8 i = 1; i < ShopItemAmount; i++)
		{
			LocalNumberOfRows = i;
			if (LocalNumberOfRows * LocalNumberOfSlotsPerRow >= ShopItemAmount)
			{
				break;
			}
		}
		InventoryComponent->NumberOfRowsInventory = LocalNumberOfRows;
		InventoryComponent->SlotsPerRowInventory = LocalNumberOfSlotsPerRow;
	}

	if (HasAuthority())
	{
		InitializeInventory();
		S_NumberOfRows = InventoryComponent->NumberOfRowsInventory;
		S_SlotsPerRow = InventoryComponent->SlotsPerRowInventory;
		S_InventorySize = S_SlotsPerRow * S_NumberOfRows;
	}
	Action = FText::FromString("Shopping");

}

bool AShopActor::InitializeInventory()
{
	if (HasAuthority())
	{
		InventoryItems = GetShopItems();
		LoadInventoryItems(InventoryItems.Num(), InventoryItems);

		return true;
	}
	return false;
}

bool AShopActor::LoadInventoryItems(uint8 Size, TArray<FSlotStructure> Items)
{
	if (HasAuthority())
	{
		S_InventorySize = Size;
		InventoryComponent->LoadInventoryItems(S_InventorySize, Items);
		return true;
	}
	return false;
}

TArray<APlayerState*> AShopActor::GetPlayersViewing_Implementation()
{
	return PlayersViewing;
}

bool AShopActor::ShopLooted_Implementation()
{
	if (HasAuthority())
	{
		TArray<APlayerState*> LocalViewers = PlayersViewing;
		IsUsable = false;
		for (APlayerState* State : LocalViewers)
		{
			AVICTIMSPlayerController* Controller = Cast<AVICTIMSPlayerController>(State->GetOwner());
			Controller->InventoryManagerComponent->Server_CloseShop();
		}
		return true;
	}
	return false;
}

bool AShopActor::GetCanShopItems_Implementation()
{
	return S_CanShopItems;
}

void AShopActor::GetShopInventory_Implementation(UInventoryComponent*& OutInventoryComponent)
{
	OutInventoryComponent = InventoryComponent;
}

void AShopActor::GetShopProperties_Implementation(FName& Namee, uint8& NumberOfRows, uint8& SlotsPerRow, bool& IsShop, uint8& InventorySize)
{
	Namee = S_Name;
	NumberOfRows = S_NumberOfRows;
	SlotsPerRow = S_SlotsPerRow;
	IsShop = S_CanShopItems;
	InventorySize = S_InventorySize;
}

bool AShopActor::OnActorUsed_Implementation(APlayerController* Controller)
{
	if (HasAuthority())
	{
		if(IsUsable)
		{
			if (AVICTIMSPlayerController* PlayerController = Cast<AVICTIMSPlayerController>(Controller))
			{
				if (PlayersViewing.Contains(PlayerController->PlayerState))
				{
					PlayerController->InventoryManagerComponent->Server_CloseShop();
					PlayerController->SetInputDependingFromVisibleWidgets();
					return true;
				}
				PlayersViewing.Add(PlayerController->PlayerState);
				PlayerController->InventoryManagerComponent->Server_UseShop(this);
				return Super::OnActorUsed_Implementation(Controller);
			}
		}
	}
	return false;
}

TArray<FShopList> AShopActor::GetShopList()
{
	TArray<FShopList> ShopItems{};
	if (!IsValid(BP_ShopDB))
	{
		BP_ShopDB = LoadObject<UDataTable>(this, TEXT("/Script/Engine.DataTable'/Game/Item/Data/DT_ShopList.DT_ShopList'"));
	}
	TArray<FName> ShopData = BP_ShopDB->GetRowNames();
	for (FName NameData : ShopData)
	{
		FShopList* ShopItem = BP_ShopDB->FindRow<FShopList>(NameData, "", true);
		ShopItems.Add(*ShopItem);
	}
	return ShopItems;
}

TArray<FSlotStructure> AShopActor::GetShopItems()
{
	TArray<FSlotStructure> ShoppingItems{};
	if (HasAuthority())
	{
		uint8 LocalShopAmount = ShopItemAmount;
		TArray<FShopList> LootShopItems = GetShopList();
		uint8 LocalShopCount = 0;
		uint8 LocalItemIndex = 0;
		TArray<uint8> LocalItemIndexes;

		if(!IsValid(DB_ItemList))
		{
			DB_ItemList = LoadObject<UDataTable>(this, TEXT("/Script/Engine.DataTable'/Game/Item/Data/Item_DB.Item_DB'"));
		}
// 			do 
// 			{
// 				LocalItemIndex = FMath::RandRange(0, LootShopItems.Num()-1);	
// 				if (LocalItemIndexes.Find(LocalItemIndex) == -1)
// 				{
// 					FShopList Shop = LootShopItems[LocalItemIndex];
// 
// 					LocalItemIndexes.AddUnique(LocalItemIndex);
// 					FItemStructure* LocalInventoryItem = DB_ItemList->FindRow<FItemStructure>(Shop.ID,"",true);
// 
// 					FSlotStructure LocalInventorySlot{};
// 					LocalInventorySlot.InitSlot(*LocalInventoryItem, 0);
// 
// 					uint8 LocalItemAmount = FMath::RandRange(1,Shop.Amount);    
// 					if (LocalItemAmount > GetItemMaxStackSize(LocalInventorySlot))
// 					{
// 						LocalItemAmount = GetItemMaxStackSize(LocalInventorySlot);
// 					}
// 					SetItemAmount(LocalInventorySlot, LocalItemAmount);
// 					ShoppingItems.Add(LocalInventorySlot);
// 					LocalShopCount++;
// 				}
// 			} while (LocalShopCount < 3);
		for (uint8 i = 0; i < LootShopItems.Num()-1; i++)				// FShopList 에 들어있는 모든 아이템요소 로드 
		{
			FShopList Shop = LootShopItems[i];
			LocalItemIndexes.AddUnique(i);
			FItemStructure* LocalInventoryItem = DB_ItemList->FindRow<FItemStructure>(Shop.ID, "", true);
			if (LocalInventoryItem->ItemType == EItemType::Undefined)
			{
				continue;
			}

			FSlotStructure LocalInventorySlot{};
			LocalInventorySlot.InitSlot(*LocalInventoryItem, 0);
			SetItemAmount(LocalInventorySlot, 1);
			ShoppingItems.Add(LocalInventorySlot);
		}

		InventoryComponent->NumberOfRowsInventory = LootShopItems.Num()-1;

		uint8 LocalNumberOfSlotsPerRow = InventoryComponent->SlotsPerRowInventory;
		InventoryComponent->SlotsPerRowInventory = LocalNumberOfSlotsPerRow;

		InventoryComponent->Server_InitInventory(InventoryComponent->NumberOfRowsInventory);

		int iTemp = 0;
	}
	return ShoppingItems;
}

void AShopActor::SetItemAmount(FSlotStructure& Item, uint8 NewAmount)
{
	Item.Amount = NewAmount;
}

uint8 AShopActor::GetItemMaxStackSize(const FSlotStructure Item)
{
	return Item.ItemStructure.MaxStackSize;
}

void AShopActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AShopActor, Body);
	DOREPLIFETIME(AShopActor, Chest);
	DOREPLIFETIME(AShopActor, Feet);
	DOREPLIFETIME(AShopActor, Hands);
	DOREPLIFETIME(AShopActor, Legs);
	DOREPLIFETIME(AShopActor, Head);
}

bool AShopActor::BeginOutlineFocus_Implementation()
{
	Super::BeginOutlineFocus_Implementation();

	Body->SetRenderCustomDepth(true);
	Body->SetCustomDepthStencilValue(2);

	Chest->SetRenderCustomDepth(true);
	Chest->SetCustomDepthStencilValue(2);

	Feet->SetRenderCustomDepth(true);
	Feet->SetCustomDepthStencilValue(2);

	Hands->SetRenderCustomDepth(true);
	Hands->SetCustomDepthStencilValue(2);

	Legs->SetRenderCustomDepth(true);
	Legs->SetCustomDepthStencilValue(2);

	Head->SetRenderCustomDepth(true);
	Head->SetCustomDepthStencilValue(2);

	return true;
}

bool AShopActor::EndOutlineFocus_Implementation()
{
	Super::EndOutlineFocus_Implementation();

	Body->SetRenderCustomDepth(false);
	Body->SetCustomDepthStencilValue(0);

	Chest->SetRenderCustomDepth(false);
	Chest->SetCustomDepthStencilValue(0);

	Feet->SetRenderCustomDepth(false);
	Feet->SetCustomDepthStencilValue(0);

	Hands->SetRenderCustomDepth(false);
	Hands->SetCustomDepthStencilValue(0);

	Legs->SetRenderCustomDepth(false);
	Legs->SetCustomDepthStencilValue(0);

	Head->SetRenderCustomDepth(false);
	Head->SetCustomDepthStencilValue(0);

	return true;
}
