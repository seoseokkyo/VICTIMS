

#include "AVICTIMSPlayerController.h"
#include "InventoryComponent.h"
#include "VICTIMSCharacter.h"
#include "PickUp.h"
#include "Blueprint/UserWidget.h"
#include "EquipmentComponent.h"
#include "InventoryManagerComponent.h"
#include "ContainerPanel.h"
#include "InventoryPanel.h"
#include "HotbarPanel.h"
#include "MainMenu.h"
#include "MainHUD.h"
#include "ItemDataStructs.h"


AVICTIMSPlayerController::AVICTIMSPlayerController()
{
	inventoryManagerComponent = CreateDefaultSubobject<UInventoryManagerComponent>(TEXT("InventoryComponent"));
	inventoryManagerComponent->SetIsReplicated(true);

	playerInventoryComponent->CreateDefaultSubobject<UEquipmentComponent>(TEXT("Equipment Component"));

	inventoryManagerComponent->NumberOfRowsInventory = playerInventoryComponent->NumberOfRowsInventory;
	inventoryManagerComponent->SlotsPerRowInventory = playerInventoryComponent->SlotsPerRowInventory;

	if (playerInventoryComponent->NumberOfSlotsOnHotbar > GetMaximumHotbarSlots())
	{
		playerInventoryComponent->NumberOfSlotsOnHotbar = GetMaximumHotbarSlots();
	}
	inventoryManagerComponent->NumberOfSlotsOnHotbar = playerInventoryComponent->NumberOfSlotsOnHotbar;
	victimsPlayer = nullptr;
	bReplicates = true;

}
void AVICTIMSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	DisableInput(this);

	FTimerHandle timerHandle;
	GetWorld()->GetTimerManager().SetTimer(timerHandle, [&]() {
		
		victimsPlayer = Cast<AVICTIMSCharacter>(GetPawn());
		playerInventoryComponent->EquipmentCharacterReference = victimsPlayer;
		inventoryManagerComponent->InitializeInventoryManager(playerInventoryComponent);

		inventoryManagerComponent->Server_InitInventory();
		SetupHUDReferences();
		EnableInput(this);

		}, 0.5f, false);
}

int AVICTIMSPlayerController::GetCurrentViewMode(const APlayerController* PlayerController)
{
	if (IsValid(PlayerController))
	{
		UGameViewportClient* gameViewportClient = PlayerController->GetWorld()->GetGameViewport();
		ULocalPlayer* localPlayer = PlayerController->GetLocalPlayer();

		bool ignore = gameViewportClient->IgnoreInput();
		EMouseCaptureMode capt = gameViewportClient->GetMouseCaptureMode();

		if (ignore == false && capt == EMouseCaptureMode::CaptureDuringMouseDown)	// Game & UI
		{
			return  0;
		}
		else if (ignore == true && capt == EMouseCaptureMode::NoCapture)			// UI ¸¸
		{
			return 1;
		}
		else
		{
																					// Game ¸¸
		}
		{
			return 2;
		}
	}

	return -1;
}

void AVICTIMSPlayerController::EnableUIMode()
{
	if (!IsValid(HUD_Reference))
	{
		return;
	}
	if (!bShowMouseCursor)
	{
		SetInputMode(FInputModeGameAndUI());
		bShowMouseCursor = true;

		SetMouseToCenterPosition();
	}
}

void AVICTIMSPlayerController::DisableUIMode()
{
	if (!IsValid(HUD_Reference))
	{
		return;
	}
	if (bShowMouseCursor)
	{
		SetInputDependingFromVisibleWidgets();
	}
}

UDataTable* AVICTIMSPlayerController::GetItemDB()
{
	return inventoryManagerComponent->GetItemDB();
}

uint8 AVICTIMSPlayerController::UIGetPlayerGold()
{
	return inventoryManagerComponent->Gold;
}

void AVICTIMSPlayerController::SetupHUDReferences()
{
	if (AMainHUD* HUDReferenceResult = Cast<AMainHUD>(GetHUD()))
	{
		HUD_Reference = HUDReferenceResult;
		HUDLayoutReference = HUDReferenceResult->MainMenuWidget;
		inventoryManagerComponent->InitializeInventoryManagerUI(HUDLayoutReference);
		inventoryManagerComponent->Client_LoadInventoryUI();
		inventoryManagerComponent->Client_LoadProfileUI();
		inventoryManagerComponent->Client_LoadHotbarUI();		
	}
}

bool AVICTIMSPlayerController::IsContainerOpen()
{
	return HUD_Reference->MainMenuWidget->Container->IsVisible();

}

void AVICTIMSPlayerController::SetInputDependingFromVisibleWidgets()
{
}

void AVICTIMSPlayerController::SetMouseToCenterPosition()
{
	int32 SizeX;
	int32 SizeY;
	GetViewportSize(SizeX, SizeY);

	SetMouseLocation(SizeX / 2, SizeY / 2);
}

void AVICTIMSPlayerController::UI_MoveHotbarItem_Implementation(const uint8& FromSlot, const uint8& ToSlot, const bool IsDraggedFromInventory, const bool IsDraggedFromHotbar)
{
	inventoryManagerComponent->Client_MoveHotbarSlotItem(FromSlot, ToSlot, IsDraggedFromInventory, IsDraggedFromHotbar);
}

void AVICTIMSPlayerController::UI_UnEquipToContainer_Implementation(const uint8& FromInventorySlot, const uint8& ToInventorySlot)
{
	inventoryManagerComponent->Server_UnEquipToContainer(FromInventorySlot, ToInventorySlot);

}

void AVICTIMSPlayerController::UI_EquipFromContainer_Implementation(const uint8& FromInventorySlot, const uint8& ToInventorySlot)
{
	inventoryManagerComponent->Server_EquipFromContainer(FromInventorySlot, ToInventorySlot);
}

void AVICTIMSPlayerController::UI_MoveContainerItem_Implementation(const uint8& FromInventorySlot, const uint8& ToInventorySlot)
{
	inventoryManagerComponent->Server_MoveContainerItem(FromInventorySlot, ToInventorySlot);
}

void AVICTIMSPlayerController::UI_DepositContainerItem_Implementation(const uint8& FromInventorySlot, const uint8& ToInventorySlot)
{
	inventoryManagerComponent->Server_DepositContainerItem(FromInventorySlot, ToInventorySlot);
}

void AVICTIMSPlayerController::UI_TakeContainerItem_Implementation(const uint8& FromContainerSlot, const uint8& ToInventorySlot)
{
	inventoryManagerComponent->Server_TakeContainerItem(FromContainerSlot, ToInventorySlot);
}

void AVICTIMSPlayerController::UI_UseHotbarItem_Implementation(const uint8& InventorySlot)
{
	inventoryManagerComponent->Client_UseHotbarSlot(InventorySlot);
}

void AVICTIMSPlayerController::UI_UseContainerItem_Implementation(const uint8& InventorySlot)
{
	inventoryManagerComponent->Server_UseContainerItem(InventorySlot);
}

void AVICTIMSPlayerController::UI_UseInventoryItem_Implementation(const uint8& InventorySlot)
{
	inventoryManagerComponent->Server_UseInventoryItem(InventorySlot);
}

void AVICTIMSPlayerController::UI_UnEquipInventoryItem_Implementation(const uint8& FromInventorySlot, const uint8& ToInventorySlot)
{
	inventoryManagerComponent->Server_UnEquipToInventory(FromInventorySlot, ToInventorySlot);
}

void AVICTIMSPlayerController::UI_EquipInventoryItem_Implementation(const uint8& FromInventorySlot, const uint8& ToInventorySlot)
{
	inventoryManagerComponent->Server_EquipFromInventory(FromInventorySlot, ToInventorySlot);
}

void AVICTIMSPlayerController::UI_DropInventoryItem_Implementation(const uint8& InventorySlot)
{
	inventoryManagerComponent->Server_DropItemFromInventory(InventorySlot);
}

void AVICTIMSPlayerController::UI_MoveInventoryItem_Implementation(const uint8& FromInventorySlot, const uint8& ToInventorySlot)
{
	inventoryManagerComponent->Server_MoveInventoryItem(FromInventorySlot, ToInventorySlot);
}

