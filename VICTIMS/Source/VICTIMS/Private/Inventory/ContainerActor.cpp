
#include "ContainerActor.h"
#include "AVICTIMSPlayerController.h"
#include "GameFramework/PlayerState.h"
#include "InventoryManagerComponent.h"

AContainerActor::AContainerActor()
{
	PrimaryActorTick.bCanEverTick = false;

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	InventoryComponent->NumberOfRowsInventory = 3;
	InventoryComponent->SlotsPerRowInventory = 3;

	C_Name = "NULL";
	C_CanStoreItems = true;
}

void AContainerActor::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		C_NumberOfRows = InventoryComponent->NumberOfRowsInventory;
		C_SlotsPerRow = InventoryComponent->SlotsPerRowInventory;

		C_InventorySize = C_SlotsPerRow * C_NumberOfRows;

		InitializeInventory();
	}
}

bool AContainerActor::InitializeInventory()
{
	if (HasAuthority())
	{
		InventoryComponent->Server_InitInventory(C_InventorySize);

		return true;
	}

	return false;
}

bool AContainerActor::LoadInventoryItems(uint8 Size, TArray<FSlotStructure> InventoryItems)
{
	if (HasAuthority())
	{
		C_InventorySize = Size;

		InventoryComponent->LoadInventoryItems(C_InventorySize, InventoryItems);

		return true;
	}

	return false;
}

bool AContainerActor::GetCanStoreItems_Implementation()
{
	return C_CanStoreItems;
}

bool AContainerActor::ContainerLooted_Implementation()
{
	if (HasAuthority())
	{
		TArray<APlayerState*> LocalViewers = PlayersViewing;
		IsUsable = false;

		for (APlayerState* State : LocalViewers)
		{
			AVICTIMSPlayerController* Controller = Cast<AVICTIMSPlayerController>(State->GetOwner());
			Controller->InventoryManagerComponent->Server_CloseContainer();
		}

		return true;

	}

	return false;
}

TArray<APlayerState*> AContainerActor::GetPlayersViewing_Implementation()
{
	return PlayersViewing;
}

void AContainerActor::GetContainerInventory_Implementation(UInventoryComponent*& OutInventoryComponent)
{
	OutInventoryComponent = InventoryComponent;
}

void AContainerActor::GetContainerProperties_Implementation(FName& Namee, uint8& NumberOfRows, uint8& SlotsPerRow, bool& IsStorageContainer, uint8& InventorySize)
{
	Namee = C_Name;
	NumberOfRows = C_NumberOfRows;
	SlotsPerRow = C_SlotsPerRow;
	IsStorageContainer = C_CanStoreItems;
	InventorySize = C_InventorySize;
}

bool AContainerActor::OnActorUsed_Implementation(APlayerController* Controller)
{
	if (HasAuthority())
	{
		if (IsUsable)
		{
			if (AVICTIMSPlayerController* PlayerController = Cast<AVICTIMSPlayerController>(Controller))
			{
				if (PlayersViewing.Contains(PlayerController->PlayerState))
				{
					PlayerController->InventoryManagerComponent->Server_CloseContainer();

					PlayerController->SetInputDependingFromVisibleWidgets();

					return true;
				}

				PlayersViewing.Add(PlayerController->PlayerState);

				PlayerController->InventoryManagerComponent->Server_UseContainer(this);

				return Super::OnActorUsed_Implementation(Controller);
				//return true;
			}
		}
	}

	return false;
}
