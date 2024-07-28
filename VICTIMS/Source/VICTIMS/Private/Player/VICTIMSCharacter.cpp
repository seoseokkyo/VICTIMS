// Copyright Epic Games, Inc. All Rights Reserved.

#include "VICTIMSCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h>
#include "CombatComponent.h"
#include "BaseWeapon.h"
#include "Components/SphereComponent.h"
#include "AVICTIMSPlayerController.h"
#include "HousingComponent.h"
#include "UsableActor.h"
#include "UsableActorInterface.h"
#include "WorldActor.h"
#include "Blueprint/UserWidget.h"
#include "InventoryComponent.h"
#include "InventoryManagerComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "InteractiveText_Panel.h"
#include "EquipmentComponent.h"
#include "HPWidget.h"
#include "PlayerDiedWidget.h"
#include "InteractText.h"
#include "TestSaveGame.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/GameplayStatics.h>
#include "Shelter.h"
#include "UI/HUDLayout.h"
#include "DropMoneyLayout.h"
#include "CollisionComponent.h"
#include "VICTIMSGameMode.h"
#include "GameFramework/PlayerState.h"
#include <../../../../../../../Source/Runtime/Engine/Public/EngineUtils.h>
#include "KillWidget.h"
#include "UI/MainLayout.h"
#include <../../../../../../../Source/Runtime/UMG/Public/Animation/WidgetAnimation.h>
#include "CompassWedget.h"
#include "MiniMapWidget.h"
#include "UI/MyHUD.h"

//<<
#include <filesystem>
#include <fstream>
#include <iostream>
//>>

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AVICTIMSCharacter

AVICTIMSCharacter::AVICTIMSCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 80.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	characterName = TEXT("Player");

	HousingComponent = CreateDefaultSubobject<UHousingComponent>(TEXT("HousingComp"));
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	InteractionField = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionField"));
	InteractionField->SetupAttachment(GetMesh());

	InteractionField->InitSphereRadius(150);
	InteractionField->SetCollisionProfileName(TEXT("CollisionTrigger"));

	// 	MainWeaponMesh = nullptr;
	ChestMesh = nullptr;
	FeetMesh = nullptr;
	// 	// 	HandsMesh = nullptr;
	LegsMesh = nullptr;
	HeadMesh = nullptr;

	//che
	collisionComponent = CreateDefaultSubobject<UCollisionComponent>(TEXT("CollisionComponent"));
	// Init Audio Resource
	//static ConstructorHelpers::FObjectFinder<USoundWave> myAudioResource(TEXT("<Reference_Path>"));
	//MatchReadyAudioWave = matchReadyAudioResource.Object;
	//KillSound = myAudioResource.Object;
	//UGameplayStatics::PlaySound2D(UObject * GetWorld(), class USoundBase* Sound, float VolumeMultiplier, float PitchMultiplier, float StartTime)

}

void AVICTIMSCharacter::BeginPlay()
{
	Super::BeginPlay();


	FActorSpawnParameters spawnParam;
	spawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	spawnParam.TransformScaleMethod = ESpawnActorScaleMethod::MultiplyWithRoot;
	spawnParam.Owner = this;
	spawnParam.Instigator = this;

	if (HousingComponent)
	{
		HousingComponent->Camera = FollowCamera; // 초기화 시 카메라 컴포넌트를 할당
	}

	if (GetController())
	{
		if (GetController()->IsLocalController())
		{
			if (auto conCheck = Cast<AVICTIMSPlayerController>(GetController()))
			{
				MyPlayerController = conCheck;
				SetOwner(MyPlayerController);
			}
		}
	}

	InteractionField->OnComponentBeginOverlap.AddDynamic(this, &AVICTIMSCharacter::OnBeginOverlap);
	InteractionField->OnComponentEndOverlap.AddDynamic(this, &AVICTIMSCharacter::OnEndOverlap);
	InteractionField->ComponentTags.Add(TEXT("InteractionField"));

	InteractionField->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel4, ECR_Ignore);
	InteractionField->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECR_Ignore);

	if (hpWidget_bp)
	{
		if (MyPlayerController && MyPlayerController->IsLocalController())
		{
			hpWidget = Cast<UHPWidget>(CreateWidget(GetWorld(), hpWidget_bp));
			//hpWidget->AddToViewport();
		}
	}

	stateComp->UpdateStat();
	if (IsLocallyControlled())
	{
		MyPlayerController->HUDLayoutReference->MainLayout->CompassWidget->AddToViewport();
		MyPlayerController->HUDLayoutReference->MainLayout->CompassWidget->SetVisibility(ESlateVisibility::Hidden);
		MyPlayerController->HUDLayoutReference->MainLayout->MiniMapWidget->SetVisibility(ESlateVisibility::Hidden);
	}


	//==========================================================================================================

		// 	MainWeapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	// 	MainWeapon->SetupAttachment(GetMesh());
	// 	MainWeapon->SetIsReplicated(true);
	FTimerHandle Timer;
	GetWorld()->GetTimerManager().ClearTimer(Timer);
	GetWorld()->GetTimerManager().SetTimer(Timer, [&]() {
		Chest = Cast<USkeletalMeshComponent>(FindComponentByTag<USkeletalMeshComponent>(FName("Chest")));
		if (Chest)
		{
			Chest->SetIsReplicated(true);
		}

		// 	Hands = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Hands"));
		// 	Hands->SetupAttachment(GetMesh());
		// 	Hands->SetIsReplicated(true);

		Feet = Cast<USkeletalMeshComponent>(FindComponentByTag<USkeletalMeshComponent>(FName("Feet")));
		if (Feet)
		{
			Feet->SetIsReplicated(true);
		}

		Legs = Cast<USkeletalMeshComponent>(FindComponentByTag<USkeletalMeshComponent>(FName("Bottom")));
		if (Legs)
		{
			Legs->SetIsReplicated(true);
		}

		Head = Cast<USkeletalMeshComponent>(FindComponentByTag<USkeletalMeshComponent>(FName("Head")));
		if (Head)
		{
			Head->SetIsReplicated(true);
		}

		// Load.....
		//<<
		if (MyPlayerController)
		{
			if (MyPlayerController->bNeedToLoad)
			{
				AVICTIMSPlayerController* PC = Cast<AVICTIMSPlayerController>(GetGameInstance()->GetFirstLocalPlayerController());

				if (PC)
				{
					FString strID = MyPlayerController->playerName;

					if (PC->CharacterReference)
					{
						PC->CharacterReference->SavePersonalID(strID);
					}

					PC->SavePersonalID(strID);
					PC->LoadData(strID);

					if (PC->CharacterReference)
					{
						if (PC->CharacterReference->AssignedHouse)
						{
							PC->CharacterReference->AssignedHouse->SetPlayerName(strID);
						}
					}

					MyPlayerController->bNeedToLoad = false;
				}
			}
		}
		//>>

		}, 0.5f, false);
}

void AVICTIMSCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (0)
	{
		PrintInfo();
	}

	if (UsableActorsInsideRange.Num() == 0)
	{
		if (IsValid(MyPlayerController))
		{
			MyPlayerController->DisableUIMode();
		}
		return;
	}
	if (IsLocallyControlled())
	{
		for (AActor*& UsableActor : UsableActorsInsideRange)
		{
			if (Cast<AWorldActor>(UsableActor))
			{
				return;
			}

			if (AUsableActor* TempUsableActor = Cast<AUsableActor>(UsableActor))
			{
				if (IUsableActorInterface::Execute_GetIsActorUsable(TempUsableActor))
				{
					FVector2D ScreenPosition = {};
					MyPlayerController->ProjectWorldLocationToScreen(UsableActor->GetActorLocation(), ScreenPosition);
					TempUsableActor->SetScreenPosition(ScreenPosition);
					if (MyPlayerController->ProjectWorldLocationToScreen(UsableActor->GetActorLocation(), ScreenPosition))
					{
						if (nullptr != TempUsableActor->InteractUserWidget)
						{
							if (TempUsableActor->InteractUserWidget->GetVisibility() == ESlateVisibility::Hidden)
							{
								TempUsableActor->InteractUserWidget->SetVisibility(ESlateVisibility::Visible);
								TempUsableActor->InteractUserWidget->SetVisibility(ESlateVisibility::Hidden);
							}
						}

						TempUsableActor->SetScreenPosition(ScreenPosition);
					}
					else
					{
						if (nullptr != TempUsableActor->InteractUserWidget)
						{
							TempUsableActor->InteractUserWidget->SetVisibility(ESlateVisibility::Hidden);
						}
					}
				}
				else {
					IUsableActorInterface::Execute_EndOutlineFocus(TempUsableActor);

					if (nullptr != TempUsableActor->InteractUserWidget)
					{
						TempUsableActor->InteractUserWidget->SetVisibility(ESlateVisibility::Hidden);
					}
				}
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
// Input

void AVICTIMSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	//if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	//{
	//	EnhancedInputComponent->ClearActionBindings();
	//}

	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// Add Input Mapping Context

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (GetController())
		{
			if (GetController()->IsLocalController())
			{
				if (auto conCheck = Cast<AVICTIMSPlayerController>(GetController()))
				{
					MyPlayerController = conCheck;
					SetOwner(MyPlayerController);
				}
			}
		}

		// Jumping
		//EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AVICTIMSCharacter::CharacterJump);
		//EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		//EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AVICTIMSCharacter::Move);

		// Looking
		//EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AVICTIMSCharacter::Look);

		EnhancedInputComponent->BindAction(ia_ToggleCombat, ETriggerEvent::Started, this, &AVICTIMSCharacter::ToggleCombat);
		EnhancedInputComponent->BindAction(ia_LeftClickAction, ETriggerEvent::Started, this, &AVICTIMSCharacter::LeftClick);

		EnhancedInputComponent->BindAction(MoveObjectAction, ETriggerEvent::Started, this, &AVICTIMSCharacter::OnMoveObject);
		EnhancedInputComponent->BindAction(RemoveObjectAction, ETriggerEvent::Started, this, &AVICTIMSCharacter::OnRemoveObject);
		/********/
		EnhancedInputComponent->BindAction(HousingBuildAction, ETriggerEvent::Started, this, &AVICTIMSCharacter::OnRightMouseButtonPressed);
		
		EnhancedInputComponent->BindAction(SaveAction, ETriggerEvent::Started, this, &AVICTIMSCharacter::SaveDataNow);


		//상호작용 =====================================================================================================================
		if (MyPlayerController == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("MyPlayerController == nullptr"));
		}
		else
		{
			EnhancedInputComponent->BindAction(Interact, ETriggerEvent::Started, MyPlayerController, &AVICTIMSPlayerController::Interact);
			EnhancedInputComponent->BindAction(ToggleProfile, ETriggerEvent::Started, MyPlayerController, &AVICTIMSPlayerController::ToggleProfile);
			EnhancedInputComponent->BindAction(ToggleInventory, ETriggerEvent::Started, MyPlayerController, &AVICTIMSPlayerController::ToggleInventory);
			EnhancedInputComponent->BindAction(ToggleMenu, ETriggerEvent::Started, MyPlayerController, &AVICTIMSPlayerController::ToggleMenu);
			EnhancedInputComponent->BindAction(ToggleUIMode, ETriggerEvent::Started, MyPlayerController, &AVICTIMSPlayerController::EnableUIMode);
			EnhancedInputComponent->BindAction(ToggleUIMode, ETriggerEvent::Started, MyPlayerController, &AVICTIMSPlayerController::DisableUIMode);
			EnhancedInputComponent->BindAction(UserHotbar1, ETriggerEvent::Started, MyPlayerController, &AVICTIMSPlayerController::UseHotbarSlot1);
			EnhancedInputComponent->BindAction(UserHotbar2, ETriggerEvent::Started, MyPlayerController, &AVICTIMSPlayerController::UseHotbarSlot2);
			EnhancedInputComponent->BindAction(UserHotbar3, ETriggerEvent::Started, MyPlayerController, &AVICTIMSPlayerController::UseHotbarSlot3);
			EnhancedInputComponent->BindAction(UserHotbar4, ETriggerEvent::Started, MyPlayerController, &AVICTIMSPlayerController::UseHotbarSlot4);
			EnhancedInputComponent->BindAction(UserHotbar5, ETriggerEvent::Started, MyPlayerController, &AVICTIMSPlayerController::UseHotbarSlot5);
			EnhancedInputComponent->BindAction(CloseLayoutAction, ETriggerEvent::Started, MyPlayerController, &AVICTIMSPlayerController::CloseLayouts);
			EnhancedInputComponent->BindAction(MoneyingAction, ETriggerEvent::Started, this, &AVICTIMSCharacter::Moneying);
		}
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AVICTIMSCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AVICTIMSCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

float AVICTIMSCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	return 0.0f;
}

void AVICTIMSCharacter::DieFunction()
{
	auto param = GetMesh()->GetCollisionResponseToChannels();
	param.SetResponse(ECC_Visibility, ECollisionResponse::ECR_Block);

	GetMesh()->SetCollisionResponseToChannels(param);

	AVICTIMSPlayerController* PC = Cast<AVICTIMSPlayerController>(GetOwner());		// 인벤토리+장비중 아이템 모두 드롭 
	if (PC)
	{
		TArray<FSlotStructure> Items = PC->InventoryManagerComponent->PlayerInventory->Inventory;
		for (int i = 0; i < Items.Num(); i++)
		{
			PC->InventoryManagerComponent->DropItem(PC->InventoryManagerComponent->PlayerInventory, i);
		}
		for (int i = 0; i < PC->InventoryManagerComponent->Gold; i++)
		{
			FSlotStructure LocalSlot = PC->InventoryManagerComponent->PlayerInventory->GetItemFromItemDB(FName("ID_Coin"));
			UClass* LocalClass = nullptr;
			FTransform OutTransform;
			PC->InventoryManagerComponent->RandomizeDropLocation(LocalSlot, LocalClass, OutTransform);
			AWorldActor* WActor = GetWorld()->SpawnActor<AWorldActor>(LocalClass, OutTransform);
			if (WActor)
			{
				WActor->StaticMesh->SetSimulatePhysics(true);
				WActor->Amount = 1;
				PC->InventoryManagerComponent->AddGold(-1);
			}
		}
	}

	if (IsLocallyControlled())
	{
		DiedWidget = CreateWidget<UPlayerDiedWidget>(GetWorld(), DiedWidget_bp);
		DiedWidget->AddToViewport(0);
		FollowCamera->PostProcessSettings.ColorSaturation = FVector4(0, 0, 0, 1);
		if (DiedWidget->DeadSound)
		{
			UGameplayStatics::PlaySound2D(GetWorld(), DiedWidget->DeadSound);
		}

		auto pc = Cast<APlayerController>(Controller);

		if (pc)
		{
			stateComp->ServerRPC_SetStatePoint(HP, stateComp->runningStat.MaxHP);  // HP max 로 채워주고 바로 자동 저장
			PC->SaveData();				
			
			pc->bShowMouseCursor = true;
			DisableInput(pc);
		}
	}

	motionState = ECharacterMotionState::Die;
	
	Super::DieFunction();

	EnableRagdoll();
																			
}


void AVICTIMSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AVICTIMSCharacter, UsableActorsInsideRange);

	// 	DOREPLIFETIME(AVICTIMSCharacter, MainWeaponMesh);
	DOREPLIFETIME(AVICTIMSCharacter, ChestMesh);
	DOREPLIFETIME(AVICTIMSCharacter, FeetMesh);
	// 	DOREPLIFETIME(AVICTIMSCharacter, HandsMesh);
	DOREPLIFETIME(AVICTIMSCharacter, LegsMesh);
	DOREPLIFETIME(AVICTIMSCharacter, HeadMesh);

	// 	DOREPLIFETIME(AVICTIMSCharacter, MainWeapon);
	DOREPLIFETIME(AVICTIMSCharacter, Chest);
	// 	DOREPLIFETIME(AVICTIMSCharacter, Hands);
	DOREPLIFETIME(AVICTIMSCharacter, Feet);
	DOREPLIFETIME(AVICTIMSCharacter, Legs);
	DOREPLIFETIME(AVICTIMSCharacter, Head);

	DOREPLIFETIME(AVICTIMSCharacter, DefaultChestMesh);
	DOREPLIFETIME(AVICTIMSCharacter, DefaultFeetMesh);
	DOREPLIFETIME(AVICTIMSCharacter, DefaultHeadMesh);
	DOREPLIFETIME(AVICTIMSCharacter, DefaultLegsMesh);

	DOREPLIFETIME(AVICTIMSCharacter, PersonalID);
}


void AVICTIMSCharacter::CharacterJump(const FInputActionValue& Value)
{
	if (motionState != ECharacterMotionState::Idle)
	{
		return;
	}

	Super::Jump();
}

void AVICTIMSCharacter::ToggleCombat(const FInputActionValue& Value)
{
	/*
	auto mainWeaponPtr = combatComponent->GetMainWeapon();
	if (IsValid(mainWeaponPtr))
	{
		if (motionState == ECharacterMotionState::Idle)
		{
			UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("combatComponent->bCombatEnable : %s"), combatComponent->bCombatEnable ? TEXT("TRUE") : TEXT("FALSE")));

			ServerRPC_ToggleCombat();
		}
	}
	*/
}

void AVICTIMSCharacter::LeftClick(const FInputActionValue& Value)
{
	if (false == combatComponent->bCombatEnable)
	{
		return;
	}

	if (combatComponent->bAttacking)
	{
		combatComponent->bAttackSaved = true;
	}
	else
	{
		AttackEvent();
	}
}

void AVICTIMSCharacter::OnRightMouseButtonPressed(const FInputActionValue& Value)
{
	if (HousingComponent && HousingComponent->IsBuildModeOn && HousingComponent->CanBuild)
	{
		Server_SpawnBuild(HousingComponent, HousingComponent->IsMoving, HousingComponent->MoveableActor, HousingComponent->BuildTransform, HousingComponent->Buildables, HousingComponent->BuildID);
	}
}

void AVICTIMSCharacter::ServerRPC_ToggleCombat_Implementation()
{
	motionState = ECharacterMotionState::ToggleCombat;

	combatComponent->bCombatEnable = !combatComponent->bCombatEnable;

	NetMulticastRPC_ToggleCombat(combatComponent->bCombatEnable);
}

void AVICTIMSCharacter::NetMulticastRPC_ToggleCombat_Implementation(bool bCombatEnable)
{
	/*auto mainWeaponPtr = combatComponent->GetMainWeapon();

	combatComponent->bCombatEnable = bCombatEnable;

	float animPlayTime = 0.0f;

	if (combatComponent->bCombatEnable)
	{
		if (mainWeaponPtr->exitCombatMontage)
		{
			animPlayTime = PlayAnimMontage(mainWeaponPtr->exitCombatMontage, 1.5f);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ToggleCombatFunction : %d"), __LINE__);
		}
	}
	else
	{
		if (mainWeaponPtr->enterCombatMontage)
		{
			animPlayTime = PlayAnimMontage(mainWeaponPtr->enterCombatMontage, 1.5f);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ToggleCombatFunction : %d"), __LINE__);
		}
	}

	//<< SSK 이거 먹히는지 테스트는 해봐야 됨
	FTimerHandle timerHandle;

	GetWorldTimerManager().SetTimer(timerHandle, [&]()
		{
			motionState = ECharacterMotionState::Idle;

			GetWorld()->GetTimerManager().ClearTimer(timerHandle);

			UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("combatComponent->bCombatEnable : %s"), combatComponent->bCombatEnable ? TEXT("TRUE") : TEXT("FALSE")));

		}, animPlayTime, false, 1.0f);
		*/
}

void AVICTIMSCharacter::PrintInfo()
{
	// localRole
	FString localRole = UEnum::GetValueAsString(GetLocalRole());

	// remoteRole
	FString remoteRole = UEnum::GetValueAsString(GetRemoteRole());

	// owner
	FString owner = GetOwner() ? GetOwner()->GetName() : "";

	// netConn
	FString netConn = GetNetConnection() ? "Valid" : "Invalid";

	FString hasController = Controller ? TEXT("HasController") : TEXT("NoController");

	FString strHP = FString::Printf(TEXT("%f"), stateComp->GetStatePoint(EStateType::HP));
	FString strSP = FString::Printf(TEXT("%f"), stateComp->GetStatePoint(EStateType::SP));

	//FString str = FString::Printf(TEXT("localRole : %s\nremoteRole : %s\nowner : %s\nnetConn : %s\nnetMode : %s\nhasController : %s\n HP : %s\n SP : %s"), *localRole, *remoteRole, *owner, *netConn, /**netMode,*/ *hasController, *strHP, *strSP);

	FString strTemp = UEnum::GetValueAsString((EVictimsNetMode)GEngine->GetNetMode(GetWorld()));

	FString str = FString::Printf(TEXT("localRole : %s\nremoteRole : %s\nowner : %s\nnetConn : %s\nhasController : %s\n HP : %s\n SP : %s\n NetMode : %s"), *localRole, *remoteRole, *owner, *netConn, *hasController, *strHP, *strSP, *strTemp);

	FVector loc = GetActorLocation() + FVector(0, 0, 50);

	DrawDebugString(GetWorld(), loc, str, nullptr, FColor::White, 0, true);
}

void AVICTIMSCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (NewController)
	{
		if (NewController->IsLocalController())
		{
			if (auto conCheck = Cast<AVICTIMSPlayerController>(NewController))
			{
				MyPlayerController = conCheck;
				SetOwner(MyPlayerController);
			}
		}
	}
}

void AVICTIMSCharacter::Moneying()
{
	if(IsLocallyControlled())
	{
		ServerRPC_AddGoldFunction(50);
	}
}

void AVICTIMSCharacter::ServerRPC_AddGoldFunction_Implementation(const int32& Gold)
{
	AVICTIMSPlayerController* con = Cast<AVICTIMSPlayerController>(MyPlayerController);
	if (con)
	{
		con->InventoryManagerComponent->AddGold(Gold);
	}
}

void AVICTIMSCharacter::OnRep_MainChestMesh()
{
	Chest->SetSkeletalMesh(ChestMesh, false);
}

void AVICTIMSCharacter::OnRep_MainFeetMesh()
{
	Feet->SetSkeletalMesh(FeetMesh, false);
}

void AVICTIMSCharacter::OnRep_MainLegsMesh()
{
	Legs->SetSkeletalMesh(LegsMesh, false);
}

void AVICTIMSCharacter::OnRep_MainHeadMesh()
{
	Head->SetSkeletalMesh(HeadMesh, false);
}

void AVICTIMSCharacter::SetChsetMesh(USkeletalMesh* NewChestMesh)
{
	ChestMesh = NewChestMesh;
	if (NewChestMesh == nullptr)
	{
		ChestMesh = DefaultChestMesh;
	}
	OnRep_MainChestMesh();
}

void AVICTIMSCharacter::SetFeetMesh(USkeletalMesh* NewFeetMesh)
{
	FeetMesh = NewFeetMesh;
	if (NewFeetMesh == nullptr)
	{
		FeetMesh = DefaultFeetMesh;
	}
	OnRep_MainFeetMesh();
}

void AVICTIMSCharacter::SetLegsMesh(USkeletalMesh* NewLegsMesh)
{
	LegsMesh = NewLegsMesh;
	if (NewLegsMesh == nullptr)
	{
		NewLegsMesh = DefaultLegsMesh;
	}
	OnRep_MainLegsMesh();
}

void AVICTIMSCharacter::SetHeadMesh(USkeletalMesh* NewHeadMesh)
{
	HeadMesh = NewHeadMesh;
	if (NewHeadMesh == nullptr)
	{
		HeadMesh = DefaultHeadMesh;
	}
	OnRep_MainHeadMesh();
}

void AVICTIMSCharacter::OnBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsLocallyControlled() && OverlappedComp == InteractionField)
	{
		if (OtherActor && (OtherActor != this) && OtherComp)
		{
			// GetUsableActor
			bool bDoesImplementInterface = OtherActor->Implements<UUsableActorInterface>();
			if (bDoesImplementInterface)
			{
				if (IsValid(OtherActor))
				{
					if (AWorldActor* WorldActor = Cast<AWorldActor>(OtherActor))
					{
						WorldActorsInsideRange.Add(WorldActor);
						UsableActorsInsideRange.Add(WorldActor);

						IUsableActorInterface::Execute_BeginOutlineFocus(WorldActor);

						SetActorTickEnabled(true);

						return;
					}

					if (AUsableActor* UsableActor = Cast<AUsableActor>(OtherActor))
					{
						UsableActorsInsideRange.Add(UsableActor);

						if (IUsableActorInterface::Execute_GetIsActorUsable(UsableActor))
						{
							IUsableActorInterface::Execute_BeginOutlineFocus(UsableActor);

							if (!UsableActor->InteractUserWidget)
							{
								FText MessageText = IUsableActorInterface::Execute_GetUseActionText(UsableActor);

								FString a = MessageText.ToString();
								UE_LOG(LogTemp, Warning, TEXT("%s"), *a);

								UUserWidget* Entry = MyPlayerController->CreateInteractWidget("InteractiveText_Entry_WBP");
								UUserWidget* Panel = MyPlayerController->CreateInteractWidget("InteractiveText_Panel_WBP");

								if (UInteractiveText_Entry* a1 = Cast<UInteractiveText_Entry>(Entry))
								{
									a1->SetNameLabelText(MessageText);
									FString string1 = MessageText.ToString();
									FName name1 = *string1;

									if (UInteractiveText_Panel* a2 = Cast<UInteractiveText_Panel>(Panel))
									{
										a2->AddEntryToList(a1);
										a2->AddToViewport();

										UsableActor->InteractUserWidget = a2;
									}
								}

							}
							UsableActor->InteractUserWidget->SetVisibility(ESlateVisibility::Visible);
							SetActorTickEnabled(true);
						}

						return;
					}
				}
			}
			else if (OtherComp->ComponentHasTag(TEXT("Door")))
			{
				CurrentDoorComponent = OtherComp;
			}
		}
	}
}


void AVICTIMSCharacter::OnEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (IsLocallyControlled() && OverlappedComp == InteractionField)
	{
		if (OtherActor && (OtherActor != this) && OtherComp)
		{
			bool bDoesImplementInterface = OtherActor->Implements<UUsableActorInterface>();
			if (bDoesImplementInterface)
			{
				if (IsValid(OtherActor))
				{
					if (AWorldActor* WorldActor = Cast<AWorldActor>(OtherActor))
					{
						IUsableActorInterface::Execute_EndOutlineFocus(WorldActor);

						WorldActorsInsideRange.Remove(WorldActor);
						UsableActorsInsideRange.Remove(WorldActor);

						return;
					}

					if (AUsableActor* UsableActor = Cast<AUsableActor>(OtherActor))
					{
						IUsableActorInterface::Execute_EndOutlineFocus(UsableActor);
						UsableActor->InteractUserWidget->SetVisibility(ESlateVisibility::Hidden);

						UsableActorsInsideRange.Remove(UsableActor);

						// At the moment, Containers are the only case that run the code until here
						if (MyPlayerController->IsContainerOpen())
						{
							MyPlayerController->InventoryManagerComponent->Server_CloseContainer();
							MyPlayerController->SetInputMode(FInputModeGameOnly());
							MyPlayerController->bShowMouseCursor = false;
						}
						if (MyPlayerController->IsShopOpen())
						{
							MyPlayerController->InventoryManagerComponent->Server_CloseShop();
							MyPlayerController->SetInputMode(FInputModeGameOnly());
							MyPlayerController->bShowMouseCursor = false;
						}
						if(MyPlayerController->HUD_Reference->IsAnyWidgetVisible())
						{
							MyPlayerController->HUDLayoutReference->MainLayout->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
							MyPlayerController->SetInputMode(FInputModeGameOnly());
							MyPlayerController->bShowMouseCursor = false;
						}
						return;
					}
				}
			}
			if (OtherComp == CurrentDoorComponent)
			{
				CurrentDoorComponent = nullptr;
			}
		}
	}
}

void AVICTIMSCharacter::Server_SpawnBuild_Implementation(UHousingComponent* Comp, bool Moving, AActor* Movable, const FTransform& Transform, const TArray<FBuildablesStructs>& DB, int32 ID)
{
	if (Comp)
	{
		// UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("AHousingTestCharacter :: Server_SpawnBuild :: Movable : %p"), Movable));
		Comp->SpawnBuild(Moving, Movable, Transform, DB, ID);
	}
}

void AVICTIMSCharacter::OnRemoveObject()
{
	if (HousingComponent)
	{
		Server_RemoveObject();
	}
}

void AVICTIMSCharacter::Server_RemoveObject_Implementation()
{
	if (HousingComponent)
	{
		HousingComponent->RemoveObject();
// 		Multicast_RemoveObject();
	}
}


void AVICTIMSCharacter::Multicast_RemoveObject_Implementation()
{
// 	if (HousingComponent)
// 	{
// 		HousingComponent->RemoveObject();
// 	}
}


void AVICTIMSCharacter::OnMoveObject()
{
	if (HousingComponent)
	{
		Server_MoveObject();
	}
}

void AVICTIMSCharacter::Server_MoveObject_Implementation()
{
	if (HousingComponent)
	{
		HousingComponent->Server_MoveObject();
		Multicast_MoveObject();
	}
}


void AVICTIMSCharacter::Multicast_MoveObject_Implementation()
{
	if (HousingComponent)
	{
		HousingComponent->MoveObject();
	}
}

void AVICTIMSCharacter::DestroyComponent(UActorComponent* TargetComponent)
{
	if (TargetComponent)
	{
		TargetComponent->DestroyComponent();
	}
}

//=======================================================================================================
// Save

void AVICTIMSCharacter::OnRep_PersonalID()
{
	UE_LOG(LogTemp, Warning, TEXT("MOVE: OnRep_PersonalID: %s"), *PersonalID);

	// PlayerState에 PersonalID를 설정
	if (APlayerState* PS = GetPlayerState())
	{
		PS->SetPlayerName(PersonalID);
	}
}

void AVICTIMSCharacter::SavePersonalID(FString ID)
{
	ServerRPC_SavePersonalID(ID);
}

void AVICTIMSCharacter::ServerRPC_SavePersonalID_Implementation(const FString& ID)
{
	PersonalID = ID;

	NetMulticastRPC_SavePersonalID(PersonalID);
}

void AVICTIMSCharacter::NetMulticastRPC_SavePersonalID_Implementation(const FString& ID)
{
	PersonalID = ID;
}

void AVICTIMSCharacter::SaveDataNow()
{
	// 	GEngine->AddOnScreenDebugMessage(2, 3.f, FColor::Blue, "Saved");

	MyPlayerController->SaveData();
}

void AVICTIMSCharacter::TestDataNow()
{
	FString Options = GetWorld()->GetLocalURL();
	FString ParsedValue;

	if (FParse::Value(*Options, TEXT("TestName"), ParsedValue))
	{
		UE_LOG(LogTemp, Warning, TEXT("Name Value: %s"), *ParsedValue);
	}

	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("%s"), *ParsedValue));
}

void AVICTIMSCharacter::SavePlayerData(UTestSaveGame* Data)
{
	if (IsLocallyControlled())
	{
		if (Data == MyPlayerController->GetSaveDataFromID(PersonalID))			// 플레이어가 갖고있는 ID 의 데이터 가져오기
		{
			Data = Cast<UTestSaveGame>(UGameplayStatics::LoadGameFromSlot(PersonalID, 0));
			Data->SavedHP = stateComp->runningStat.currentHP;					  // 현재 플레이어 HP 저장
			Data->SavedGold = MyPlayerController->InventoryManagerComponent->Gold; // 현재 인벤토리 Gold 저장
			// 			UE_LOG(LogTemp, Warning, TEXT("Save Player Data ---------- Successed"));
		}
		else
		{
			// 			UE_LOG(LogTemp, Warning, TEXT("Save Player Data ---------- Failed"));
		}
	}
}

void AVICTIMSCharacter::LoadPlayerData(UTestSaveGame* Data)
{
	if (IsLocallyControlled())
	{
		if (Data)
		{
			// 			SavedData = Data;	// 데이터 변수 저장/
			stateComp->ServerRPC_SetStatePoint(EStateType::HP, Data->SavedHP);	// 플레이어 HP 로드
			MyPlayerController->InventoryManagerComponent->AddGold(Data->SavedGold);	// Gold 로드

			// 			UE_LOG(LogTemp, Warning, TEXT("SetStatePoint HP : %f"), Data->SavedHP);
			// 			UE_LOG(LogTemp, Warning, TEXT("AddGold : %d"), Data->SavedGold);
		}
		else
		{
			//UE_LOG(LogTemp, Warning, TEXT("LoadPlayerData %s"), IsValid(Data) ? TEXT("Success") : TEXT("Failed"));
		}
	}
}

void AVICTIMSCharacter::SaveHousingData(FName playerName)
{
}

void AVICTIMSCharacter::AddHousingData(FName playerName)
{
}

void AVICTIMSCharacter::LoadHousingData(FName playerName)
{
}

void AVICTIMSCharacter::KillWidgetOn(ACharacterBase* DiedChar)
{
	ServerRPC_KillWidget_Implementation(DiedChar);
}

void AVICTIMSCharacter::ServerRPC_KillWidget_Implementation(ACharacterBase* DiedChar)
{
	NetMulticastRPC_KillWidget_Implementation(DiedChar);
}

void AVICTIMSCharacter::NetMulticastRPC_KillWidget_Implementation(ACharacterBase* DiedChar)
{
	MyPlayerController = Cast<AVICTIMSPlayerController>(GetController());
	if (MyPlayerController)
	{
		MyPlayerController->HUDLayoutReference->MainLayout->KillWidget->PlayAnimation(MyPlayerController->HUDLayoutReference->MainLayout->KillWidget->KillAnimation);

		UGameplayStatics::PlaySound2D(GetWorld(), KillSound, 1, 1, 0);
		//UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("is killed UI MyPlayerController")), true, true, FColor(1, 1, 1), 30.0f);

	}
	//UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("is killed UI IsLocallyControlled")), true, true, FColor(1, 1, 1), 30.0f);

//UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("is killed UI NetMulticastRPC_KillWidget_Implementation")), true, true, FColor(1, 1, 1), 30.0f);

}

void AVICTIMSCharacter::SetAssignedHouse(AShelter* NewHouse)
{
	ServerRPC_SetAssignedHouse(NewHouse);
}

void AVICTIMSCharacter::ServerRPC_SetAssignedHouse_Implementation(AShelter* NewHouse)
{
	AssignedHouse = NewHouse;

	ClientRPC_SetAssignedHouse(AssignedHouse);
}

void AVICTIMSCharacter::ClientRPC_SetAssignedHouse_Implementation(AShelter* NewHouse)
{
	AssignedHouse = NewHouse;
}

void AVICTIMSCharacter::Server_GoToHouse_Implementation()
{
	if (AssignedHouse)
	{
		ClientRPC_GoToHouse(AssignedHouse->OriginPos);
	}
}

void AVICTIMSCharacter::ClientRPC_GoToHouse_Implementation(FVector houseLocation)
{
	if (houseLocation != FVector::ZeroVector)
	{
		SetActorLocation(houseLocation);
	}
}


void AVICTIMSCharacter::Server_GoToOtherHouse_Implementation(const FString& otherPlayerName)
{
	//auto gm = GetWorld()->GetAuthGameMode<AVICTIMSGameMode>();

	//for (auto house : gm->Houses)
	//{
	//	if (house->OwnerPlayerName == otherPlayerName)
	//	{			
	//		ClientRPC_GoToHouse(house->OriginPos);
	//		break;
	//	}
	//}	

	//for (TActorIterator<AShelter> it(GetWorld()); it; ++it)
	//{
	//	if ((*it)->OwnerPlayerName == otherPlayerName)
	//	{
	//		ClientRPC_GoToHouse((*it)->OriginPos);
	//		break;
	//	}
	//}
	
	for (auto player : MyPlayerController->otherPlayers)
	{
		if (player->playerName == otherPlayerName)
		{
			ClientRPC_GoToHouse(player->CharacterReference->AssignedHouse->OriginPos);
			break;
		}
	}
}

void AVICTIMSCharacter::ClientRPC_GoToOtherHouse_Implementation(FVector houseLocation)
{
	if (houseLocation != FVector::ZeroVector)
	{
		SetActorLocation(houseLocation);
	}
}
