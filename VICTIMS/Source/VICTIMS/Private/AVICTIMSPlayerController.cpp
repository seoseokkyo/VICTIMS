

#include "AVICTIMSPlayerController.h"
#include "VICTIMSCharacter.h"
#include "MyHUD.h"
#include "WorldActor.h"
#include "Blueprint/UserWidget.h"
#include "EquipmentComponent.h"
#include "InventoryManagerComponent.h"
#include "ContainerLayout.h"
#include "HUDLayout.h"
#include "InteractiveText_Panel.h"
#include "MainLayout.h"
#include "UsableActor.h"
#include "InventoryComponent.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h>
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/GameplayStatics.h>
#include "ShopLayout.h"
#include "TestSaveGame.h"
#include "TestIDWIdget.h"
#include "Components/TextBlock.h"
#include "IDInValidWidget.h"
#include "SavedWidget.h"
#include "Net/UnrealNetwork.h"
#include "Shelter.h"
#include "System/VICTIMSGameMode.h"
#include "UI/InventoryLayout.h"
#include "UI/ProfileLayout.h"
#include "DropMoneyLayout.h"

AVICTIMSPlayerController::AVICTIMSPlayerController()
{
	InventoryManagerComponent = CreateDefaultSubobject<UInventoryManagerComponent>(TEXT("InventoryComponent"));
	InventoryManagerComponent->SetIsReplicated(true);

	PlayerInventoryComponent = CreateDefaultSubobject<UEquipmentComponent>(TEXT("EquipmentComponent"));

	InventoryManagerComponent->NumberOfRowsInventory = PlayerInventoryComponent->NumberOfRowsInventory;
	InventoryManagerComponent->SlotsPerRowInventory = PlayerInventoryComponent->SlotsPerRowInventory;

	if (PlayerInventoryComponent->NumberOfSlotsOnHotbar > GetMaximumHotbarSlots())
	{
		PlayerInventoryComponent->NumberOfSlotsOnHotbar = GetMaximumHotbarSlots();
	}
	InventoryManagerComponent->NumberOfSlotsOnHotbar = PlayerInventoryComponent->NumberOfSlotsOnHotbar;

	CharacterReference = nullptr;

	bReplicates = true;
}

void AVICTIMSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	//if (CharacterReference == nullptr)
	//{
	//	CharacterReference = Cast<AVICTIMSCharacter>(GetPawn());

	//	UE_LOG(LogTemp, Warning, TEXT("CharacterReference Was Null, Replace : %p"), CharacterReference);
	//}

	if (IsLocalController())					// ID 입력 위젯  
	{
		if (TestIDWidget_bp)
		{
			TestIDWidget = Cast<UTestIDWidget>(CreateWidget(GetWorld(), TestIDWidget_bp));
			if (TestIDWidget)
			{
				bIsShowUI = true;
				TestIDWidget->AddToViewport();
				SetInputMode(FInputModeUIOnly());
				EnableUIMode();
				bShowMouseCursor = true;
			}
		}
		if (IDInvalidWidget_bp)
		{
			IDInValidWidget = Cast<UIDInValidWidget>(CreateWidget(GetWorld(), IDInvalidWidget_bp));
			if (IDInValidWidget)
			{
				IDInValidWidget->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
	}
}

int AVICTIMSPlayerController::GetCurrentViewMode(const APlayerController* PlayerController)
{
	if (IsValid(PlayerController))
	{
		UGameViewportClient* GameViewportClient = PlayerController->GetWorld()->GetGameViewport();
		if (GameViewportClient)
		{
			ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();

			bool ignore = GameViewportClient->IgnoreInput();
			EMouseCaptureMode capt = GameViewportClient->GetMouseCaptureMode();

			if (ignore == false && capt == EMouseCaptureMode::CaptureDuringMouseDown)
			{
				return 0;  // Game And UI
			}
			else if (ignore == true && capt == EMouseCaptureMode::NoCapture)
			{
				return 1;  // UI Only
			}
			else
			{
				return 2;  // Game Only
			}
		}
		else
		{

		}
	}
	return -1;
}

void AVICTIMSPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

}

void AVICTIMSPlayerController::Tick(float DeltaTime)
{
	auto pawnCheck = GetPawn();
	if (pawnCheck != nullptr)
	{
		auto charCheck = Cast<AVICTIMSCharacter>(pawnCheck);

		if (CharacterReference == nullptr || CharacterReference != charCheck)
		{
			CharacterReference = charCheck;

			UE_LOG(LogTemp, Warning, TEXT("Inventory,Controller Init(%s)"), *GetPawn()->GetActorNameOrLabel());

			PlayerInventoryComponent->EquipmentCharacterReference = CharacterReference;

			InventoryManagerComponent->InitializeInventoryManager(PlayerInventoryComponent);
			InventoryManagerComponent->Server_InitInventory();
			InventoryManagerComponent->InitializePlayerAttributes();

			//CharacterReference->TestFunction(CharacterReference->InputComponent);

			CharacterReference->EnableInput(this);
		}
	}

	int viewModeCheck = GetCurrentViewMode(this);

	if (viewModeCheck == 0)
	{
		GEngine->AddOnScreenDebugMessage(2, 3.f, FColor::Red, "Game And UI");
	}
	else if (viewModeCheck == 1)
	{
		GEngine->AddOnScreenDebugMessage(2, 3.f, FColor::Red, "UI Only");
	}
	else if (viewModeCheck == 2)
	{
		GEngine->AddOnScreenDebugMessage(2, 3.f, FColor::Red, "Game Only");
	}
}

uint8 AVICTIMSPlayerController::UIGetPlayerGold()
{
	return InventoryManagerComponent->Gold;
}


void AVICTIMSPlayerController::SetupHUDReferences()
{
	if (IsLocalController())
	{
		if (AMyHUD* HUDReferenceResult = Cast<AMyHUD>(GetHUD()))
		{
			HUD_Reference = HUDReferenceResult;

			HUDLayoutReference = HUDReferenceResult->HUDReference;

			InventoryManagerComponent->InitializeInventoryManagerUI(HUDLayoutReference->MainLayout);

			InventoryManagerComponent->Client_LoadInventoryUI();
			InventoryManagerComponent->Client_LoadProfileUI();
			InventoryManagerComponent->Client_LoadHotbarUI();
		}
	}
}


void AVICTIMSPlayerController::CollectFromPanel(const FName& Name)
{
	for (AActor*& Actor : CharacterReference->UsableActorsInsideRange)
	{
		if (AWorldActor* WorldActor = Cast<AWorldActor>(Actor))
		{
			if (WorldActor->ID == Name)
			{
				Server_OnActorUsed(WorldActor);

				return;
			}
		}
		else
		{
			Server_OnActorUsed(Actor);
			return;
		}
	}
}


bool AVICTIMSPlayerController::IsContainerOpen()
{
	return HUD_Reference->HUDReference->MainLayout->Container->IsVisible();
}


void AVICTIMSPlayerController::ToggleInventory()
{
	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("ToggleInventory Pressed"));

	if (IsValid(HUD_Reference))
	{
		HUD_Reference->ToggleWindow(ELayout::Inventory);
		SetInputDependingFromVisibleWidgets();
	}
}


void AVICTIMSPlayerController::ToggleProfile()
{
	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("ToggleProfile Pressed"));

	if (IsValid(HUD_Reference))
	{
		HUD_Reference->ToggleWindow(ELayout::Equipment);
		SetInputDependingFromVisibleWidgets();
	}
}


void AVICTIMSPlayerController::SetInputDependingFromVisibleWidgets()
{
	if (IsLocalPlayerController())
	{
		if (bIsShowUI == false)
		{

			if (HUD_Reference->IsAnyWidgetVisible())
			{
				SetInputMode(FInputModeGameAndUI());
				bShowMouseCursor = true;
				
				HUDLayoutReference->MainLayout->SetVisibility(ESlateVisibility::Visible);
			}
			else
			{
				SetInputMode(FInputModeGameOnly());
				bShowMouseCursor = false;
				
				HUDLayoutReference->MainLayout->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}
		}
	}
}


void AVICTIMSPlayerController::ToggleContainer()
{
	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("ToggleContainer Pressed"));

	if (IsValid(HUD_Reference))
	{
		HUD_Reference->ToggleWindow(ELayout::Container);
		SetInputDependingFromVisibleWidgets();
	}
}


bool AVICTIMSPlayerController::IsShopOpen()
{
	return HUD_Reference->HUDReference->MainLayout->Shop->IsVisible();
}
void AVICTIMSPlayerController::ToggleShop()
{
	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("ToggleShop Pressed"));

	if (IsValid(HUD_Reference))
	{
		HUD_Reference->ToggleWindow(ELayout::Shop);
		SetInputDependingFromVisibleWidgets();
	}
}

void AVICTIMSPlayerController::ToggleMenu()
{

}

void AVICTIMSPlayerController::UI_PerChaseItem_Implementation(const uint8& InventorySlot)
{
	InventoryManagerComponent->Server_PerchaseShopItem(InventorySlot);
}

void AVICTIMSPlayerController::UI_SellItem_Implementation(const uint8& InventorySlot)
{
	InventoryManagerComponent->Server_SellItem(InventorySlot);
}

void AVICTIMSPlayerController::Server_OnActorUsed_Implementation(AActor* Actor)
{
	OnActorUsed(Actor);
}


UUserWidget* AVICTIMSPlayerController::GenerateInteractWidget(FText Text)
{
	if (HUD_Reference == nullptr)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("HUD_Reference : %p"), HUD_Reference));

		return nullptr;
	}
	else
	{
		return HUD_Reference->GenerateInteractWidget(Text);
	}
}


UUserWidget* AVICTIMSPlayerController::CreateInteractWidget(FName Name)
{
	if (HUD_Reference == nullptr)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("HUD_Reference : %p"), HUD_Reference));

		return nullptr;
	}
	else
	{
		return HUD_Reference->CustomCreateWidget(Name);
	}
}


void AVICTIMSPlayerController::SetMouseToCenterPosition()
{
	int32 SizeX;
	int32 SizeY;
	GetViewportSize(SizeX, SizeY);

	SetMouseLocation(SizeX / 2, SizeY / 2);
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
	if (bIsShowUI == false)
	{
		if (!IsValid(HUD_Reference) || bUseUIMode)
		{
			return;
		}

		if (bShowMouseCursor)
		{
			SetInputDependingFromVisibleWidgets();

		}
	}
}

UDataTable* AVICTIMSPlayerController::GetItemDB()
{
	return InventoryManagerComponent->GetItemDB();
}

void AVICTIMSPlayerController::UseHotbarSlot1()
{
	InventoryManagerComponent->Client_UseHotbarSlot(0);
}

void AVICTIMSPlayerController::UseHotbarSlot2()
{
	InventoryManagerComponent->Client_UseHotbarSlot(1);
}

void AVICTIMSPlayerController::UseHotbarSlot3()
{
	InventoryManagerComponent->Client_UseHotbarSlot(2);
}

void AVICTIMSPlayerController::UseHotbarSlot4()
{
	InventoryManagerComponent->Client_UseHotbarSlot(3);
}

void AVICTIMSPlayerController::UseHotbarSlot5()
{
	InventoryManagerComponent->Client_UseHotbarSlot(4);
}

void AVICTIMSPlayerController::Interact()
{
	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("Interact Pressed"));

	if (CharacterReference == nullptr)
	{
		CharacterReference = Cast<AVICTIMSCharacter>(GetPawn());
	}

	if (CharacterReference->UsableActorsInsideRange.Num() > 0)
	{
		uint32 Index = 0;
		// 		GetSelectedItemIndex(Index);

		AActor* Actor = CharacterReference->UsableActorsInsideRange[Index];

		if (AWorldActor* WorldActor = Cast<AWorldActor>(Actor))
		{
			CollectFromPanel(WorldActor->ID);

			return;
		}
		if (AUsableActor* UsableActor = Cast<AUsableActor>(Actor))
		{
			Server_OnActorUsed(UsableActor);

			return;
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Character Reference is null"))
	}
}

void AVICTIMSPlayerController::OnActorUsed(AActor* Actor1)
{
	if (HasAuthority())
	{
		if (IsValid(Actor1))
		{
			IUsableActorInterface::Execute_OnActorUsed(Actor1, this);
		}
	}
}

void AVICTIMSPlayerController::RequestClientTravel(const FString& URL, const FString& Options)
{
	ServerRPC_RequestClientTravel(URL, Options);
}

void AVICTIMSPlayerController::ServerRPC_RequestClientTravel_Implementation(const FString& URL, const FString& Options)
{
	ClientRPC_RequestClientTravel(URL, Options);
}

void AVICTIMSPlayerController::ClientRPC_RequestClientTravel_Implementation(const FString& URL, const FString& Options)
{
	UGameplayStatics::OpenLevel(this, FName(*URL), true, Options);
}

void AVICTIMSPlayerController::UI_MoveContainerItem_Implementation(const uint8& FromInventorySlot, const uint8& ToInventorySlot)
{
	InventoryManagerComponent->Server_MoveContainerItem(FromInventorySlot, ToInventorySlot);
}

void AVICTIMSPlayerController::UI_MoveHotbarItem_Implementation(const uint8& FromSlot, const uint8& ToSlot, const bool IsDraggedFromInventory, const bool IsDraggedFromHotbar)
{
	InventoryManagerComponent->Client_MoveHotbarSlotItem(FromSlot, ToSlot, IsDraggedFromInventory, IsDraggedFromHotbar);
}

void AVICTIMSPlayerController::UI_MoveInventoryItem_Implementation(const uint8& FromInventorySlot, const uint8& ToInventorySlot)
{
	InventoryManagerComponent->Server_MoveInventoryItem(FromInventorySlot, ToInventorySlot);
}

void AVICTIMSPlayerController::UI_UnEquipToContainer_Implementation(const uint8& FromInventorySlot, const uint8& ToInventorySlot)
{
	InventoryManagerComponent->Server_UnEquipToContainer(FromInventorySlot, ToInventorySlot);
}

void AVICTIMSPlayerController::UI_EquipFromContainer_Implementation(const uint8& FromInventorySlot, const uint8& ToInventorySlot)
{
	InventoryManagerComponent->Server_EquipFromContainer(FromInventorySlot, ToInventorySlot);
}

void AVICTIMSPlayerController::UI_DepositContainerItem_Implementation(const uint8& FromInventorySlot, const uint8& ToInventorySlot)
{
	InventoryManagerComponent->Server_DepositContainerItem(FromInventorySlot, ToInventorySlot);
}

void AVICTIMSPlayerController::UI_TakeContainerItem_Implementation(const uint8& FromContainerSlot, const uint8& ToInventorySlot)
{
	InventoryManagerComponent->Server_TakeContainerItem(FromContainerSlot, ToInventorySlot);
}

void AVICTIMSPlayerController::UI_UseHotbarItem_Implementation(const uint8& InventorySlot)
{
	InventoryManagerComponent->Client_UseHotbarSlot(InventorySlot);
}

void AVICTIMSPlayerController::UI_UseContainerItem_Implementation(const uint8& InventorySlot)
{
	InventoryManagerComponent->Server_UseContainerItem(InventorySlot);
}

void AVICTIMSPlayerController::UI_UseInventoryItem_Implementation(const uint8& InventorySlot)
{
	InventoryManagerComponent->Server_UseInventoryItem(InventorySlot);
}

void AVICTIMSPlayerController::UI_UnEquipInventoryItem_Implementation(const uint8& FromInventorySlot, const uint8& ToInventorySlot)
{
	InventoryManagerComponent->Server_UnEquipToInventory(FromInventorySlot, ToInventorySlot);
}

void AVICTIMSPlayerController::UI_EquipInventoryItem_Implementation(const uint8& FromInventorySlot, const uint8& ToInventorySlot)
{
	InventoryManagerComponent->Server_EquipFromInventory(FromInventorySlot, ToInventorySlot);
}

void AVICTIMSPlayerController::UI_DropInventoryItem_Implementation(const uint8& InventorySlot)
{
	InventoryManagerComponent->Server_DropItemFromInventory(InventorySlot);
}

//====================================================================================================================
// Save

void AVICTIMSPlayerController::CreateSaveData(FString ID)
{
	if (UGameplayStatics::DoesSaveGameExist(ID, 0) || ID.IsEmpty())
	{
		if (TestIDWidget)
		{
			TestIDWidget->IsIDValid = false;
			if (IDInValidWidget)
			{
				IDInValidWidget->AddToViewport();
				IDInValidWidget->ValidInformText->SetText(FText::FromString("ID IS ALREADY EXIST"));
				IDInValidWidget->SetVisibility(ESlateVisibility::Visible);

				FTimerHandle Time;
				GetWorld()->GetTimerManager().SetTimer(Time, [&]() {

					IDInValidWidget->SetVisibility(ESlateVisibility::Collapsed);
					}, 0.5f, false);
			}
		}
	}
	else
	{
		SavedData = Cast<UTestSaveGame>(UGameplayStatics::CreateSaveGameObject(UTestSaveGame::StaticClass()));
		UGameplayStatics::SaveGameToSlot(SavedData, ID, 0);
		SaveData(ID);

		if (TestIDWidget)
		{
			TestIDWidget->IsIDValid = true;
		}
	}
}

UTestSaveGame* AVICTIMSPlayerController::GetSaveDataFromID(FString ID)
{
	if (UGameplayStatics::DoesSaveGameExist(ID, 0))  // 해당 ID 이름의 저장된 데이터가 있으면 
	{
		return SavedData = Cast<UTestSaveGame>(UGameplayStatics::LoadGameFromSlot(ID, 0));	// 데이터 불러오기 
	}
	else
	{
		return nullptr;
	}
}

void AVICTIMSPlayerController::SaveData(FString ID)
{
	if (HasAuthority())
	{

		SavedData = Cast<UTestSaveGame>(UGameplayStatics::LoadGameFromSlot(ID, 0));
		if (SavedData)
		{
			SavedData->SavedHP = CharacterReference->stateComp->runningStat.currentHP;	// HP 초기값 저장 
			SavedData->SavedGold = CharacterReference->MyPlayerController->InventoryManagerComponent->Gold;	// Gold 초기값 저장
			CharacterReference->SavePersonalID(ID);

			SavedData->SavedItemIDs.Reset();
			SavedData->SavedItemAmounts.Reset();

			int startPoint = (int)EEquipmentSlot::Count;

			uint8 NumberOfRowsInventory = CharacterReference->MyPlayerController->InventoryManagerComponent->PlayerInventory->NumberOfRowsInventory;
			uint8 SlotsPerRowInventory = CharacterReference->MyPlayerController->InventoryManagerComponent->PlayerInventory->SlotsPerRowInventory;

			int inventorySize = NumberOfRowsInventory * SlotsPerRowInventory;

			for (int i = 0; i < startPoint + inventorySize; i++)
			{

				FString TempItemID = CharacterReference->MyPlayerController->InventoryManagerComponent->PlayerInventory->GetInventoryItem(i).ItemStructure.ID.ToString();
				SavedData->SavedItemIDs.Add(TempItemID);

				uint8 TempItemAmount = CharacterReference->MyPlayerController->InventoryManagerComponent->PlayerInventory->GetInventoryItem(i).Amount;
				SavedData->SavedItemAmounts.Add(TempItemAmount);
			}
			
			// 집 번호 저장
			SavedData->HouseNumber = CharacterReference->AssignedHouse ? CharacterReference->AssignedHouse->HouseNumber : -1;

			UGameplayStatics::SaveGameToSlot(SavedData, ID, 0);
			HUD_Reference->HUDReference->MainLayout->Saved->SetVisibility(ESlateVisibility::Visible);
			FTimerHandle Timer;
			GetWorld()->GetTimerManager().SetTimer(Timer, [&]() {
				HUD_Reference->HUDReference->MainLayout->Saved->SetVisibility(ESlateVisibility::Hidden);
				}, 0.5f, false);
		}
	}
}

void AVICTIMSPlayerController::LoadData(FString ID)
{
	if (IsLocalController())
	{
		if (UGameplayStatics::DoesSaveGameExist(ID, 0))
		{
			if (TestIDWidget)
			{
				TestIDWidget->IsIDValid = true;
			}

			if (AVICTIMSCharacter* p = Cast<AVICTIMSCharacter>(CharacterReference))
			{
				SavedData = Cast<UTestSaveGame>(UGameplayStatics::LoadGameFromSlot(ID, 0));
				CharacterReference->SavePersonalID(ID);
				CharacterReference->stateComp->NetMulticastRPC_SetStatePoint(EStateType::HP, SavedData->SavedHP);	// 플레이어 HP 로드
				InventoryManagerComponent->AddGold(SavedData->SavedGold);	// Gold 로드


				// 인벤토리 아이템 로드 
				int itemCount = SavedData->SavedItemIDs.Num()-1;
				int startPoint = (int)EEquipmentSlot::Count;

				for (int i = 0; i < itemCount; i++)
				{
					if (SavedData->SavedItemIDs[i].Contains(TEXT("ID_Empty")))
					{
						continue;
					}

					FSlotStructure TempSlot = InventoryManagerComponent->GetItemFromItemDB(FName(*SavedData->SavedItemIDs[i]));

					if (TempSlot.ItemStructure.ItemType == EItemType::Undefined)
					{
						continue;
					}

					TempSlot.Amount = SavedData->SavedItemAmounts[i];

					//======================================================================================================	
					// 클라이언트 아이템로드 안되는 중	

					bool bOutSuccess = false;

					CharacterReference->MyPlayerController->InventoryManagerComponent->TryToAddItemToInventory(CharacterReference->MyPlayerController->InventoryManagerComponent->PlayerInventory, TempSlot, bOutSuccess);

					// 장비중인 아이템 로드 
					for (int j = startPoint; j < startPoint + startPoint; j++)
					{
						if (TempSlot.ItemStructure.ID != FName("ID_Empty"))
						{
							CharacterReference->MyPlayerController->InventoryManagerComponent->UseInventoryItem(j);
						}
					}
				}

				// 집 번호 로드 및 할당
				if (SavedData->HouseNumber >= 0 && SavedData->HouseNumber < GameModeReference->Houses.Num())
				{
					AShelter* AssignedHouse = GameModeReference->Houses[SavedData->HouseNumber];
					CharacterReference->SetAssignedHouse(AssignedHouse);
				}

			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Load Player Data ---------- Failed333333333___Controller"));
			}
		}
		else
		{
			if (TestIDWidget)
			{

				TestIDWidget->IsIDValid = false;
				if (IDInValidWidget)
				{
					IDInValidWidget->AddToViewport();
					IDInValidWidget->ValidInformText->SetText(FText::FromString("ID IS NOT EXIST"));
					IDInValidWidget->SetVisibility(ESlateVisibility::Visible);

					FTimerHandle Time;
					GetWorld()->GetTimerManager().SetTimer(Time, [&]() {

						IDInValidWidget->SetVisibility(ESlateVisibility::Collapsed);
						}, 0.5f, false);
				}
			}
		}
	}
}

void AVICTIMSPlayerController::ServerRPC_SetUseUIState_Implementation(bool bUse)
{
	bUseUIMode = bUse;

	UE_LOG(LogTemp, Warning, TEXT("bUseUIMode : %s"), bUseUIMode ? TEXT("TRUE") : TEXT("FALSE"));

	ClientRPC_SetUseUIState(bUseUIMode);
}

void AVICTIMSPlayerController::ClientRPC_SetUseUIState_Implementation(bool bUse)
{
	bUseUIMode = bUse;
}

void AVICTIMSPlayerController::CloseTestIDWidget()	// TestIDWidget 지우기
{
	if (IsLocalController())
	{
		bIsShowUI = false;
		TestIDWidget->RemoveFromParent();
		IDInValidWidget->RemoveFromParent();
		SetInputMode(FInputModeGameOnly());
		bShowMouseCursor = false;
	}
}


void AVICTIMSPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AVICTIMSPlayerController, bUseUIMode);
}

void AVICTIMSPlayerController::CloseLayouts()
{
	if (ESlateVisibility::Visible == HUDLayoutReference->MainLayout->Inventory->GetVisibility())
	{
		HUDLayoutReference->MainLayout->Inventory->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	if (ESlateVisibility::Visible == HUDLayoutReference->MainLayout->Shop->GetVisibility())
	{
		HUDLayoutReference->MainLayout->Shop->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}	
	if (ESlateVisibility::Visible == HUDLayoutReference->MainLayout->Profile->GetVisibility())
	{
		HUDLayoutReference->MainLayout->Profile->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}	
	if (ESlateVisibility::Visible == HUDLayoutReference->MainLayout->Container->GetVisibility())
	{
		HUDLayoutReference->MainLayout->Container->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}	
	if (ESlateVisibility::Visible == HUDLayoutReference->MainLayout->DropMoneyLayout->GetVisibility())
	{
		HUDLayoutReference->MainLayout->DropMoneyLayout->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}	
// 	return;
}