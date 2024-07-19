

#include "WorldActor.h"
#include "AVICTIMSPlayerController.h"
#include "EquipmentComponent.h"
#include "InventoryManagerComponent.h"
#include "Net/UnrealNetwork.h"

AWorldActor::AWorldActor()
{
	PrimaryActorTick.bCanEverTick = false;
	ID = FName(TEXT("None"));
	Amount = 1;

}

void AWorldActor::BeginPlay()
{
	Super::BeginPlay();

	if (StaticMesh)
	{
		StaticMesh->SetSimulatePhysics(true);
		StaticMesh->SetAllMassScale(8.0f);
	}

	UDataTable* BP_ItemDB = LoadObject<UDataTable>(this, TEXT("/Game/Item/Data/Item_DB.Item_DB"));
	if (IsValid(BP_ItemDB))
	{
		ItemDB = BP_ItemDB;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("WorldActor : UDataTable not Loaded"))
	}

	Server_InitializeItemData();
}

void AWorldActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWorldActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWorldActor, WorldMesh);
}

bool AWorldActor::LoadItemFromList()
{
	FItemStructure* NewItemData = ItemDB->FindRow<FItemStructure>(FName(ID), "", true);

	if (!NewItemData)
	{
		UE_LOG(LogTemp, Warning, TEXT("Loading Item... returned NULL."));
		return false;
	}

	InventoryItem.InitSlot(*NewItemData, Amount);

	InventoryItem.Amount = Amount;
	InventoryItem.ItemStructure = *NewItemData;

	Name = NewItemData->Name;

	return true;
}

void AWorldActor::Server_InitializeItemData_Implementation()
{
	LoadItemFromList();
}

bool AWorldActor::OnActorUsed_Implementation(APlayerController* Controller)
{
	if (HasAuthority())
	{
		if (AVICTIMSPlayerController* PlayerController = Cast<AVICTIMSPlayerController>(Controller))
		{
			bool OutSuccess = false;
			UInventoryComponent* InventoryComp = PlayerController->PlayerInventoryComponent;

			UInventoryManagerComponent* InventoryManager = PlayerController->InventoryManagerComponent;
			InventoryItem.Amount = Amount;

			InventoryManager->TryToAddItemToInventory(InventoryComp, InventoryItem, OutSuccess);

			if (OutSuccess)
			{
				InventoryManager->Server_UpdateTooltips();
				Destroy();
			}

			return Super::OnActorUsed_Implementation(Controller);
		}
	}
	return false;
}
