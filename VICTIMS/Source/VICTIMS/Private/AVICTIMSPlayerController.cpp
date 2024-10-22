

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
#include "Sound/Soundbase.h"
#include "DropMoneyLayout.h"
#include "HousingNotification.h"
#include "HousingComponent.h"
#include "MovingInfoWidget.h"
#include "GameFramework/PlayerState.h"
#include "Components/EditableText.h"
#include "Components/AudioComponent.h"
#include "LoadingWidget.h"
#include "HPWidget.h"
#include "CompassWedget.h"
#include "MiniMapWidget.h"
#include "Hotbar.h"
#include "EquippingWeaponWidget.h"
#include "UI/Hotbar_Slot.h"
#include "BuildSaveData.h"
#include "SeqPlayWidget.h"
#include "MediaSoundComponent.h"
#include "TabMenuWidget.h"
#include "Components/Border.h"
#include "Components/Slider.h"
#include "HousingTutorialWidget.h"
#include "UI/InteractText.h"

AVICTIMSPlayerController::AVICTIMSPlayerController()
{
	InventoryManagerComponent = CreateDefaultSubobject<UInventoryManagerComponent>(TEXT("InventoryComponent"));
	InventoryManagerComponent->SetIsReplicated(true);

	PlayerInventoryComponent = CreateDefaultSubobject<UEquipmentComponent>(TEXT("EquipmentComponent"));

	//che
	//partycomponent = CreateDefaultSubobject<UPartyComponent>(TEXT("PartyComponent"));

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

	if (IsLocalController())					// ID 입력 위젯  
	{
		FString Options = GetWorld()->GetLocalURL();
		FString ParsedValue;

		if (FParse::Value(*Options, TEXT("TestName="), ParsedValue))
		{
			ParsedValue.RemoveFromEnd("=");
			// 			UE_LOG(LogTemp, Warning, TEXT("Name Value: %s"), *ParsedValue);
		}

		playerName = ParsedValue;

		if (TestIDWidget_bp)
		{
			TestIDWidget = Cast<UTestIDWidget>(CreateWidget(GetWorld(), TestIDWidget_bp));

			if (ParsedValue.IsEmpty())
			{
				if (TestIDWidget)
				{
					bIsShowUI = true;
					TestIDWidget->AddToViewport();
					SetInputMode(FInputModeUIOnly());
					EnableUIMode();
					bShowMouseCursor = true;

					if (BGM)
					{
						BGMComp = UGameplayStatics::CreateSound2D(GetWorld(), BGM);
						BGMComp->Play();
					}
				}
			}
			else
			{
				bNeedToLoad = true;

				ChatManagerEnable(playerName);
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

		if (TabMenu_wbp)
		{
			TabMenu = Cast<UTabMenuWidget>(CreateWidget(GetWorld(), TabMenu_wbp));
			if (TabMenu)
			{
				TabMenu->AddToViewport();
				TabMenu->MainBorder->SetVisibility(ESlateVisibility::Collapsed);
			}
		}

		//if (TutorialWidget_wbp)
		//{
		//	TutorialWidget = Cast<UHousingTutorialWidget>(CreateWidget(GetWorld(), TutorialWidget_wbp));
		//	TutorialWidget->AddToViewport();
		//	TutorialWidget->SetVisibility(ESlateVisibility::Collapsed);
		//}
	}

	if (auto gi = Cast<UVictimsGameInstance>(GetGameInstance()))
	{
		FTimerHandle timer;
		GetWorldTimerManager().SetTimer(timer, [&, gi]()
			{
				gi->ClientRPC_HideLoadingUI();
			}, 2.0f, false);
	}
}

void AVICTIMSPlayerController::HandleEscapeKey()
{
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

	if (IsLocalController())
	{
		if (GetHUD() == nullptr)
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.Owner = this;
			SpawnInfo.Instigator = GetInstigator();
			SpawnInfo.ObjectFlags |= RF_Transient;	// We never want to save HUDs into a map
			MyHUD = GetWorld()->SpawnActor<AMyHUD>(SpawnInfo);

			SetupHUDReferences();
		}
	}
}

void AVICTIMSPlayerController::Tick(float DeltaTime)
{
	auto pawnCheck = GetPawn();
	if (pawnCheck != nullptr)
	{
		auto charCheck = Cast<AVICTIMSCharacter>(pawnCheck);

		if (CharacterReference == nullptr || CharacterReference != charCheck || false == bInventoryInitialized)
		{
			CharacterReference = charCheck;
			CharacterReference->MyPlayerController = this;

			PlayerInventoryComponent->EquipmentCharacterReference = CharacterReference;

			InventoryManagerComponent->InitializeInventoryManager(PlayerInventoryComponent);
			InventoryManagerComponent->Server_InitInventory();
			InventoryManagerComponent->InitializePlayerAttributes();

			CharacterReference->EnableInput(this);

			bInventoryInitialized = true;
		}
	}
}

int32 AVICTIMSPlayerController::UIGetPlayerGold()
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
	// 	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("ToggleInventory Pressed"));

	if (IsValid(HUD_Reference))
	{
		HUD_Reference->ToggleWindow(ELayout::Inventory);
		SetInputDependingFromVisibleWidgets();
	}
}

void AVICTIMSPlayerController::ToggleProfile()
{
	// 	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("ToggleProfile Pressed"));

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
				bWidgetBlocked = true;
			}
			else
			{
				SetInputMode(FInputModeGameOnly());
				bShowMouseCursor = false;
				HUDLayoutReference->MainLayout->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				bWidgetBlocked = false;
			}
		}
	}
}

void AVICTIMSPlayerController::ToggleContainer()
{
	// 	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("ToggleContainer Pressed"));

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
	// 	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("ToggleShop Pressed"));

	if (IsValid(HUD_Reference))
	{
		HUD_Reference->ToggleWindow(ELayout::Shop);
		SetInputDependingFromVisibleWidgets();
	}
}

void AVICTIMSPlayerController::ToggleMenu()
{
	if (ESlateVisibility::Visible == TabMenu->MainBorder->GetVisibility())
	{
		if (ToggleSound)
		{
			UGameplayStatics::PlaySound2D(GetWorld(), ToggleSound);
		}
		TabMenu->OnClickCloseMenuButton();
		DisableUIMode();
		bShowMouseCursor = false;
		bIsShowUI = false;
	}
	else
	{
		if (ToggleSound)
		{
			UGameplayStatics::PlaySound2D(GetWorld(), ToggleSound);
		}
		TabMenu->MainBorder->SetVisibility(ESlateVisibility::Visible);
		EnableUIMode();
		bShowMouseCursor = true;
		bIsShowUI = true;
	}
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
		// 		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("HUD_Reference : %p"), HUD_Reference));

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
		// 		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("HUD_Reference : %p"), HUD_Reference));

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
	// 	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("Interact Pressed"));

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
	else if (CharacterReference->CurrentDoorComponent)
	{
		if (CharacterReference->HousingComponent->IsBuildModeOn)
		{
			ShowHousingNotification();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ByeDoor"));
			// 맵 변경(나가기)이 아니라 위젯 띄워주기
			// CharacterReference->SetActorLocation(FVector(1850, 821, 169));
			ShowMovingInfo();

			if (HUDLayoutReference->MainLayout->HousingTutorialWidget)
			{
				HUDLayoutReference->MainLayout->HousingTutorialWidget->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
		//return;
	}
	else {
		// 		UE_LOG(LogTemp, Warning, TEXT("Character Reference is null"));
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
	ClientRPC_ShowLoadingUI();

	SaveData();

	ServerRPC_RequestClientTravel(URL, Options);
}

void AVICTIMSPlayerController::ServerRPC_RequestClientTravel_Implementation(const FString& URL, const FString& Options)
{
	ClientRPC_RequestClientTravel(URL, Options);
}

void AVICTIMSPlayerController::ClientRPC_RequestClientTravel_Implementation(const FString& URL, const FString& Options)
{
	FString testOptions = FString::Printf(TEXT("TestName=%s &"), *playerName);

	UGameplayStatics::OpenLevel(this, FName(*URL), true, testOptions);
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

void AVICTIMSPlayerController::SavePersonalID(FString ID)
{
	ServerRPC_SavePersonalID(ID);

	// 	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Chached Name : %s"), *PlayerID));
}

void AVICTIMSPlayerController::ServerRPC_SavePersonalID_Implementation(const FString& ID)
{
	playerName = ID;

	NetMulticastRPC_SavePersonalID(playerName);
}

void AVICTIMSPlayerController::NetMulticastRPC_SavePersonalID_Implementation(const FString& ID)
{
	playerName = ID;
}

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
		SaveData();

		CharacterReference = Cast<AVICTIMSCharacter>(GetPawn());
		if (CharacterReference)
		{
			CharacterReference->SavePersonalID(ID);
			// 			UE_LOG(LogTemp, Warning, TEXT("Move :: CreateSaveData: PersonalID set to %s"), *ID);
		}
		if (PlayerState)
		{
			PlayerState->SetPlayerName(ID);
		}

		if (TestIDWidget)
		{
			TestIDWidget->IsIDValid = true;
		}

		// 서버에 플레이어 리스트 업데이트 요청
		////Server_RequestPlayerListUpdate();
	}
}

UTestSaveGame* AVICTIMSPlayerController::GetSaveDataFromID(FString ID)
{
	if (UGameplayStatics::DoesSaveGameExist(ID, 0))  // 해당 ID 이름의 저장된 데이터가 있으면 
	{
		return Cast<UTestSaveGame>(UGameplayStatics::LoadGameFromSlot(ID, 0));	// 데이터 불러오기 
	}
	else
	{
		return nullptr;
	}
}

void AVICTIMSPlayerController::SaveData()
{
	ServerRPC_SaveData();
}

void AVICTIMSPlayerController::ServerRPC_SaveData_Implementation()
{
	FString ID = playerName;

	auto saveData = Cast<UTestSaveGame>(UGameplayStatics::CreateSaveGameObject(UTestSaveGame::StaticClass()));
	UGameplayStatics::SaveGameToSlot(saveData, ID, 0);

	if (saveData)
	{
		saveData->SavedHP = CharacterReference->stateComp->runningStat.currentHP;	// HP 초기값 저장 
		saveData->SavedGold = InventoryManagerComponent->Gold;						// Gold 초기값 저장
		CharacterReference->SavePersonalID(ID);

		saveData->SavedItemIDs.Reset();
		saveData->SavedItemAmounts.Reset();
		saveData->SavedHotbarItemIDs.Reset();


		int startPoint = (int)EEquipmentSlot::Count;

		uint8 NumberOfRowsInventory = InventoryManagerComponent->PlayerInventory->NumberOfRowsInventory;
		uint8 SlotsPerRowInventory = InventoryManagerComponent->PlayerInventory->SlotsPerRowInventory;

		int inventorySize = NumberOfRowsInventory * SlotsPerRowInventory;

		for (int i = 0; i < startPoint + inventorySize; i++)
		{
			FString TempItemID = InventoryManagerComponent->PlayerInventory->GetInventoryItem(i).ItemStructure.ID.ToString();
			saveData->SavedItemIDs.Add(TempItemID);

			uint8 TempItemAmount = InventoryManagerComponent->PlayerInventory->GetInventoryItem(i).Amount;
			saveData->SavedItemAmounts.Add(TempItemAmount);
		}

		// 집 번호 저장
		saveData->HouseNumber = CharacterReference->AssignedHouse ? CharacterReference->AssignedHouse->HouseNumber : -1;

		UGameplayStatics::SaveGameToSlot(saveData, ID, 0);
		//=====================================================================================================================================
		//		Save QuickSlot
		ClientRPC_SaveHotbar(ID);
		NetMulticastRPC_SaveData();

		if (auto gm = GetWorld()->GetAuthGameMode<AVICTIMSGameMode>())
		{
			gm->SaveBuildComps(this);
		}
	}
	else
	{
		// 		UE_LOG(LogTemp, Warning, TEXT("LoadGameFromSlot:%s | Was Failed"), *ID);
	}
}

void AVICTIMSPlayerController::NetMulticastRPC_SaveData_Implementation()
{
	if (IsLocalController())
	{
		HUD_Reference->HUDReference->MainLayout->Saved->SetVisibility(ESlateVisibility::Visible);
		FTimerHandle Timer;
		GetWorld()->GetTimerManager().SetTimer(Timer, [&]() {
			HUD_Reference->HUDReference->MainLayout->Saved->SetVisibility(ESlateVisibility::Hidden);
			}, 0.5f, false);
	}
}

void AVICTIMSPlayerController::LoadData(FString ID)
{
	ServerRPC_LoadData(ID);

	ServerRPC_RestoreBuildComps();
}

void AVICTIMSPlayerController::ClientRPC_SetBuildCompsData_Implementation(const TArray<FBuildSaveDataStruct>& _buildCompsData)
{
	buildCompsData = _buildCompsData;
}

void AVICTIMSPlayerController::ServerRPC_SearchBuildComps_Implementation()
{
	if (CharacterReference)
	{
		if (CharacterReference->AssignedHouse)
		{
			buildCompsData = CharacterReference->AssignedHouse->SearchBuildItem();
			ClientRPC_SetBuildCompsData(buildCompsData);
		}
	}
}

void AVICTIMSPlayerController::ServerRPC_RestoreBuildComps_Implementation()
{
	if (auto gm = GetWorld()->GetAuthGameMode<AVICTIMSGameMode>())
	{
		gm->RestoreBuildComps(this);
	}
}

void AVICTIMSPlayerController::ServerRPC_LoadData_Implementation(const FString& ID)
{
	if (UGameplayStatics::DoesSaveGameExist(ID, 0))
	{
		if (TestIDWidget)
		{
			TestIDWidget->IsIDValid = true;
		}

		if (AVICTIMSCharacter* p = Cast<AVICTIMSCharacter>(GetPawn()))
		{
			CharacterReference = p;

			auto savedData = Cast<UTestSaveGame>(UGameplayStatics::LoadGameFromSlot(ID, 0));

			CharacterReference->SavePersonalID(ID);

			if (savedData->SavedHP != 0)
			{
				CharacterReference->stateComp->NetMulticastRPC_SetStatePoint(EStateType::HP, savedData->SavedHP);	// 플레이어 HP 로드
			}
			else
			{
				// 				UKismetSystemLibrary::PrintString(GetWorld(), TEXT("savedData->SavedHP Was Zero"));
			}

			InventoryManagerComponent->AddGold(savedData->SavedGold);	// Gold 로드

			if (PlayerState)
			{
				PlayerState->SetPlayerName(/*SaveGame->PlayerDataStructure.PlayerID*/ID);
			}

			// 인벤토리 아이템 로드 
			int itemCount = savedData->SavedItemIDs.Num() - 1;
			int startPoint = (int)EEquipmentSlot::Count;

			for (int i = 0; i < itemCount; i++)
			{
				if (savedData->SavedItemIDs[i].Contains(TEXT("ID_Empty")))
				{
					continue;
				}

				FSlotStructure TempSlot = InventoryManagerComponent->GetItemFromItemDB(FName(*savedData->SavedItemIDs[i]));

				if (TempSlot.ItemStructure.ItemType == EItemType::Undefined)
				{
					continue;
				}

				TempSlot.Amount = savedData->SavedItemAmounts[i];

				bool bOutSuccess = false;

				//InventoryManagerComponent->AddItem(InventoryManagerComponent->PlayerInventory, i, TempSlot);

				InventoryManagerComponent->TryToAddItemToInventory(InventoryManagerComponent->PlayerInventory, TempSlot, bOutSuccess);
				ClientRPC_LoadHotbar(ID, TempSlot, savedData->SavedHotbarItemIDs);

				// 장비중인 아이템 로드 
				if (TempSlot.ItemStructure.ItemType == EItemType::Equipment)
				{
					//if(startPoint < i && i < startPoint*2)
					if ((int)EEquipmentSlot::Undefined < i && i < (int)EEquipmentSlot::Count)
					{
						//InventoryManagerComponent->UseInventoryItem(i);
						InventoryManagerComponent->EquipItemAtLoad(InventoryManagerComponent->PlayerInventory, 6, InventoryManagerComponent->PlayerInventory, i);
					}
				}
			}

			//===================================================================================================================================
			//			Load QuickSlot 
			// 			for (uint8 i = 0; i < 5; i++)
			// 			{
			// 				if (savedData->SavedHotbarItemIDs[i].Contains(TEXT("ID_Empty")))
			// 				{
			// 					continue;
			// 				}
			// 				FSlotStructure TempHotbarSlot = InventoryManagerComponent->GetItemFromItemDB(FName(*savedData->SavedHotbarItemIDs[i]));
			// 
			// 				if (TempHotbarSlot.ItemStructure.ItemType == EItemType::Undefined)
			// 				{
			// 					continue;
			// 				}
			// 
			// 				for (int j = 0; j < itemCount; j++)
			// 				{
			// 					if (savedData->SavedItemIDs[j].Contains(TEXT("ID_Empty")))
			// 					{
			// 						continue;
			// 					}
			// 
			// 					FSlotStructure TempInventorySlotItem = InventoryManagerComponent->PlayerInventory->GetInventoryItem(j);
			// 
			// 					if (TempInventorySlotItem.ItemStructure.ItemType == EItemType::Undefined)
			// 					{
			// 						continue;
			// 					}
			// 
			// 					if (TempHotbarSlot.ItemStructure.ID == TempInventorySlotItem.ItemStructure.ID)
			// 					{
			// 						InventoryManagerComponent->Client_SetHotbarSlotItem(i, TempInventorySlotItem);
			// 					}
			// 				}
			// 			}
		}
		else
		{
			// 			UE_LOG(LogTemp, Warning, TEXT("Load Player Data ---------- Failed333333333___Controller"));
		}

		NetMulticastRPC_LoadData(true);
	}
	else
	{
		NetMulticastRPC_LoadData(false);
	}
}
void AVICTIMSPlayerController::ServerRPC_LoadHotbarData_Implementation(const FString& ID, const uint8& Index, const FSlotStructure& HotbarItems)
{
	// 	auto savedData = Cast<UTestSaveGame>(UGameplayStatics::LoadGameFromSlot(ID, 0));
	// 	int itemCount = savedData->SavedItemIDs.Num() - 1;
	// 	int startPoint = (int)EEquipmentSlot::Count;
	// 	 for (uint8 i = 0; i < 5; i++)
	// 	 {
	// 		if (savedData->SavedHotbarItemIDs[i].Contains(TEXT("ID_Empty")))
	// 		{
	// 			continue;
	// 		}
	// 		FSlotStructure TempHotbarSlot = InventoryManagerComponent->GetItemFromItemDB(FName(*savedData->SavedHotbarItemIDs[i]));
	// 		if (TempHotbarSlot.ItemStructure.ItemType == EItemType::Undefined)
	// 		{
	// 			continue;
	// 		}
	// 		for (int j = 0; j < itemCount; j++)
	// 		{
	// 			if (savedData->SavedItemIDs[j].Contains(TEXT("ID_Empty")))
	// 			{
	// 				continue;
	// 			}
	// 			FSlotStructure TempInventorySlotItem = InventoryManagerComponent->PlayerInventory->GetInventoryItem(j);
	// 			if (TempInventorySlotItem.ItemStructure.ItemType == EItemType::Undefined)
	// 			{
	// 				continue;
	// 			}
	// 
	// 			if (TempHotbarSlot.ItemStructure.ID == TempInventorySlotItem.ItemStructure.ID)
	// 			{
	InventoryManagerComponent->Client_SetHotbarSlotItem(Index, HotbarItems);
	// 			}
	// 		}
	// 	 }
}
void AVICTIMSPlayerController::NetMulticastRPC_LoadData_Implementation(bool bSuccess)
{
	// 	if (IsLocalController())
	// 	{
	if (TestIDWidget)
	{
		if (bSuccess)
		{
			TestIDWidget->IsIDValid = true;
			CloseTestIDWidget();

			CharacterReference->hpWidget->AddToViewport();

			HUD_Reference->HUDReference->MainLayout->CompassWidget->SetVisibility(ESlateVisibility::Visible);
			HUD_Reference->HUDReference->MainLayout->MiniMapWidget->SetVisibility(ESlateVisibility::Visible);

			if (BGMComp)
			{
				BGMComp->Stop();
			}
			if (IDInValidWidget)
			{
				IDInValidWidget->AddToViewport();
				//  					IDInValidWidget->ValidInformText->SetText(FText::FromString("Data Load Success"));
				IDInValidWidget->SetVisibility(ESlateVisibility::Visible);

				FTimerHandle Time;
				GetWorld()->GetTimerManager().SetTimer(Time, [&]() {

					IDInValidWidget->SetVisibility(ESlateVisibility::Collapsed);
					}, 0.5f, false);
			}

		}
		else
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
	// 	}
}

void AVICTIMSPlayerController::ServerRPC_SetUseUIState_Implementation(bool bUse)
{
	bUseUIMode = bUse;

	// 	UE_LOG(LogTemp, Warning, TEXT("bUseUIMode : %s"), bUseUIMode ? TEXT("TRUE") : TEXT("FALSE"));

	ClientRPC_SetUseUIState(bUseUIMode);
}

void AVICTIMSPlayerController::ClientRPC_SetUseUIState_Implementation(bool bUse)
{
	bUseUIMode = bUse;
}

void AVICTIMSPlayerController::ClientRPC_SaveHotbar_Implementation(const FString& ID)
{
	// 	auto savedData = Cast<UTestSaveGame>(UGameplayStatics::LoadGameFromSlot(ID, 0));
	// 	savedData->SavedHotbarItemIDs.Reset();
	for (uint8 i = 0; i < 5; i++)
	{
		FString TempHotbarID = InventoryManagerComponent->GetHotbarSlotItem(i).ItemStructure.ID.ToString();
		ServerRPC_SendHotbarData(ID, TempHotbarID);
		// 		savedData->SavedHotbarItemIDs.Add(TempHotbarID);
	}
	// 	UGameplayStatics::SaveGameToSlot(savedData, ID, 0);
}

void AVICTIMSPlayerController::ServerRPC_SendHotbarData_Implementation(const FString& ID, const FString& HotbarItems)
{
	auto savedData = Cast<UTestSaveGame>(UGameplayStatics::LoadGameFromSlot(ID, 0));
	savedData->SavedHotbarItemIDs.Add(HotbarItems);
	UGameplayStatics::SaveGameToSlot(savedData, ID, 0);
}

void AVICTIMSPlayerController::ClientRPC_LoadHotbar_Implementation(const FString& ID, const FSlotStructure& ComparedItem, const TArray<FString>& savedHotbarItemNames)
{
	// 	auto savedData = Cast<UTestSaveGame>(UGameplayStatics::LoadGameFromSlot(ID, 0));
	/* 	int itemCount = InventoryManagerComponent->PlayerInventory->Inventory.Num();*/
	// 	int startPoint = (int)EEquipmentSlot::Count;

	for (uint8 i = 0; i < 5; i++)
	{
		if (savedHotbarItemNames[i].Contains(TEXT("ID_Empty")))
		{
			continue;
		}
		FSlotStructure TempHotbarSlot = InventoryManagerComponent->GetItemFromItemDB(FName(*savedHotbarItemNames[i]));

		if (TempHotbarSlot.ItemStructure.ItemType == EItemType::Undefined)
		{
			continue;
		}
		// 
		// 		for (int j = 0; j < itemCount; j++)
		// 		{
		// 			if (savedData->SavedItemIDs[j].Contains(TEXT("ID_Empty")))
		// 			{
		// 				continue;
		// 			}
		// 
		// 			FSlotStructure TempInventorySlotItem = InventoryManagerComponent->PlayerInventory->GetInventoryItem(j);
		// 
		// 			if (TempInventorySlotItem.ItemStructure.ItemType == EItemType::Undefined)
		// 			{
		// 				continue;
		// 			}

		if (TempHotbarSlot.ItemStructure.ID == ComparedItem.ItemStructure.ID)
		{
			// 				ServerRPC_LoadHotbarData(ID, i, TempInventorySlotItem);
			InventoryManagerComponent->Client_SetHotbarSlotItem(i, ComparedItem);
			break;
		}
		/*}*/
	}
}

void AVICTIMSPlayerController::CloseTestIDWidget()	// TestIDWidget 지우기
{
	if (IsLocalController())
	{
		bIsShowUI = false;
		TestIDWidget->RemoveFromParent();
		IDInValidWidget->RemoveFromParent();
		DisableUIMode();
		SetInputMode(FInputModeGameOnly());
		bShowMouseCursor = false;
	}
}


void AVICTIMSPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AVICTIMSPlayerController, bUseUIMode);
	DOREPLIFETIME(AVICTIMSPlayerController, bNeedToLoad);
}

void AVICTIMSPlayerController::CloseLayouts()
{
	if (ESlateVisibility::Visible == HUDLayoutReference->MainLayout->Inventory->GetVisibility())
	{
		HUDLayoutReference->MainLayout->Inventory->SetVisibility(ESlateVisibility::Hidden);
		return;
	}
	if (ESlateVisibility::Visible == HUDLayoutReference->MainLayout->Shop->GetVisibility())
	{
		HUDLayoutReference->MainLayout->Shop->SetVisibility(ESlateVisibility::Hidden);
		return;
	}
	if (ESlateVisibility::Visible == HUDLayoutReference->MainLayout->Profile->GetVisibility())
	{
		HUDLayoutReference->MainLayout->Profile->SetVisibility(ESlateVisibility::Hidden);
		return;
	}
	if (ESlateVisibility::Visible == HUDLayoutReference->MainLayout->Container->GetVisibility())
	{
		HUDLayoutReference->MainLayout->Container->SetVisibility(ESlateVisibility::Hidden);
		return;
	}
	if (ESlateVisibility::Visible == HUDLayoutReference->MainLayout->DropMoneyLayout->GetVisibility())
	{
		HUDLayoutReference->MainLayout->DropMoneyLayout->SetVisibility(ESlateVisibility::Hidden);
		return;
	}
	if (ESlateVisibility::Visible == TabMenu->MainBorder->GetVisibility())
	{
		TabMenu->MainBorder->SetVisibility(ESlateVisibility::Collapsed);
		bIsShowUI = false;
	}
	// 	return;
}

void AVICTIMSPlayerController::ClientRPC_AddTag_Implementation(const FName& newTag)
{
	GetPawn()->Tags.Add(newTag);
}

void AVICTIMSPlayerController::HideWidgets()
{
	CharacterReference->hpWidget->SetVisibility(ESlateVisibility::Hidden);

	HUD_Reference->HUDReference->MainLayout->CompassWidget->SetVisibility(ESlateVisibility::Hidden);
	HUD_Reference->HUDReference->MainLayout->MiniMapWidget->SetVisibility(ESlateVisibility::Hidden);
	HUD_Reference->HUDReference->MainLayout->EquipWeaponWidget->SetVisibility(ESlateVisibility::Hidden);
	HUD_Reference->HUDReference->MainLayout->Hotbar->SetVisibility(ESlateVisibility::Hidden);
	HUD_Reference->HUDReference->MainLayout->Inventory->SetVisibility(ESlateVisibility::Hidden);
	HUD_Reference->HUDReference->MainLayout->Profile->SetVisibility(ESlateVisibility::Hidden);
}

void AVICTIMSPlayerController::VicTimsSeqPlay()
{
	ServerRPC_RequsetSeqPlay();
}

void AVICTIMSPlayerController::SetSeqPlayingState(bool bOn)
{
	ServerRPC_SetSeqPlayingState(bOn);
}

void AVICTIMSPlayerController::ServerRPC_SetSeqPlayingState_Implementation(bool bOn)
{
	bSeqIsPlaying = bOn;
	ClientRPC_SetSeqPlayingState(bSeqIsPlaying);
}

void AVICTIMSPlayerController::ClientRPC_SetSeqPlayingState_Implementation(bool bOn)
{
	bSeqIsPlaying = bOn;
}

void AVICTIMSPlayerController::ServerRPC_RequsetSeqPlay_Implementation()
{
	ClientRPC_RequsetSeqPlay();
}

void AVICTIMSPlayerController::ClientRPC_RequsetSeqPlay_Implementation()
{
	auto SeqWidget = CreateWidget<USeqPlayWidget>(this, bp_SeqplayWidget);
	SeqWidget->SetOwningPlayer(this);
	SeqWidget->AddToViewport();

	SeqWidget->PlayMedia();
}

void AVICTIMSPlayerController::ClientRPC_EnableHousingTipText_Implementation(bool bEnable, bool bItem, bool bHousing, bool bHousingDel)
{
	if (bEnable)
	{
		HUDLayoutReference->MainLayout->Interact->ShowInteractText(bItem, bHousing, bHousingDel);
	}
	else
	{
		HUDLayoutReference->MainLayout->Interact->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AVICTIMSPlayerController::ServerRPC_UpdatePlayerList_Implementation()
{
	otherPlayers = GetWorld()->GetAuthGameMode<AVICTIMSGameMode>()->GetPlayers();

	otherPlayerNames.Reset();
	for (auto player : otherPlayers)
	{
		otherPlayerNames.Add(player->playerName);
	}

	ClientRPC_UpdatePlayerList(otherPlayers);
	ClientRPC_UpdatePlayerNameList(otherPlayerNames);
}

void AVICTIMSPlayerController::ClientRPC_UpdatePlayerList_Implementation(const TArray<AVICTIMSPlayerController*>& otherPlayerList)
{
	otherPlayers = otherPlayerList;
}

void AVICTIMSPlayerController::ClientRPC_UpdatePlayerNameList_Implementation(const TArray<FString>& otherPlayerNameList)
{
	otherPlayerNames = otherPlayerNameList;

	OnChangedPlayerList.ExecuteIfBound(otherPlayerNames);
}



void AVICTIMSPlayerController::ShowHousingNotification()
{
	if (!HousingNotificationWidget)
	{
		if (HousingNotificationWidgetClass)
		{
			HousingNotificationWidget = CreateWidget<UHousingNotification>(this, HousingNotificationWidgetClass);
			if (HousingNotificationWidget)
			{
				HousingNotificationWidget->AddToViewport();
			}
		}
	}

	if (HousingNotificationWidget)
	{
		HousingNotificationWidget->SetVisibility(ESlateVisibility::Visible);

		GetWorld()->GetTimerManager().SetTimer(HousingNotificationTimerHandle, this, &AVICTIMSPlayerController::HideHousingNotification, 1.0f, false);
	}
}

void AVICTIMSPlayerController::HideHousingNotification()
{
	if (HousingNotificationWidget)
	{
		HousingNotificationWidget->SetVisibility(ESlateVisibility::Hidden);
	}

	GetWorld()->GetTimerManager().ClearTimer(HousingNotificationTimerHandle);
}

void AVICTIMSPlayerController::EnableHousingTutorialWidget(bool bEnable)
{
	if (HUDLayoutReference->MainLayout->HousingTutorialWidget)
	{
		HUDLayoutReference->MainLayout->HousingTutorialWidget->SetVisibility(bEnable ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

void AVICTIMSPlayerController::ServerRPC_TryReload_Implementation(FName Bullet)
{
	InventoryManagerComponent->UseBulletItem(Bullet);
}

void AVICTIMSPlayerController::ShowMovingInfo()
{
	if (IsLocalController())
	{
		if (HUDLayoutReference->MainLayout->MovingInfo->GetVisibility() == ESlateVisibility::Hidden)
		{
			HUDLayoutReference->MainLayout->MovingInfo->SetVisibility(ESlateVisibility::Visible);
			bIsShowUI = true;
			EnableUIMode();
		}
		else if (HUDLayoutReference->MainLayout->MovingInfo->GetVisibility() == ESlateVisibility::Visible)
		{
			HUDLayoutReference->MainLayout->MovingInfo->SetVisibility(ESlateVisibility::Hidden);
			bIsShowUI = false;
			DisableUIMode();
		}
	}
}

void AVICTIMSPlayerController::ClientRPC_ShowLoadingUI_Implementation()
{
	HideWidgets();

	if (bp_LoadingWidget)
	{
		if (auto gi = Cast<UVictimsGameInstance>(GetGameInstance()))
		{
			LoadingWidget = CreateWidget<ULoadingWidget>(gi, bp_LoadingWidget);

			gi->ServerRPC_ShowLoadingUI(LoadingWidget);

			//LoadingWidget->AddToViewport();
		}
	}
}
