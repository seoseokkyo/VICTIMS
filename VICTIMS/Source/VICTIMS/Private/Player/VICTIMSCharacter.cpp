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
#include "MainHUD.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AVICTIMSCharacter

AVICTIMSCharacter::AVICTIMSCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
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


	PlayerInventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("PlayerInventory"));
	PlayerInventory->SetSlotsCapacity(20);

	interactableRange = CreateDefaultSubobject<USphereComponent>(TEXT("Interactable Range"));
	interactableRange -> SetupAttachment(RootComponent);
	interactableRange -> SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	interactableRange -> SetRelativeScale3D(FVector(4));
	interactableRange->OnComponentBeginOverlap.AddDynamic(this, &AVICTIMSCharacter::OnBeginOverlapInteractableRange);

	InteractionCheckFrequency = 0.1;

	characterName = TEXT("Player");


	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AVICTIMSCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	FActorSpawnParameters spawnParam;
	spawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	spawnParam.TransformScaleMethod = ESpawnActorScaleMethod::MultiplyWithRoot;
	spawnParam.Owner = this;
	spawnParam.Instigator = this;

	ABaseWeapon* equipment = GetWorld()->SpawnActor<ABaseWeapon>(defaultWeapon, GetActorTransform(), spawnParam);

	if (equipment)
	{
		equipment->OnEquipped();
	}
	
	if (IsLocallyControlled())
	{
		MainPlayerController = Cast<AVICTIMSPlayerController>(GetController());
		HUD = Cast<AMainHUD>(MainPlayerController->GetHUD());
	}
}


void AVICTIMSCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

}
//////////////////////////////////////////////////////////////////////////
// Input

void AVICTIMSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AVICTIMSCharacter::CharacterJump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);


		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AVICTIMSCharacter::BeginInteract);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Completed, this, &AVICTIMSCharacter::EndInteract);
		EnhancedInputComponent->BindAction(ToggleMenuAction, ETriggerEvent::Started, this, &AVICTIMSCharacter::ToggleMenu);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AVICTIMSCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AVICTIMSCharacter::Look);

		EnhancedInputComponent->BindAction(ia_ToggleCombat, ETriggerEvent::Started, this, &AVICTIMSCharacter::ToggleCombat);
		EnhancedInputComponent->BindAction(ia_LeftClickAction, ETriggerEvent::Started, this, &AVICTIMSCharacter::LeftClick);
		
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

void AVICTIMSCharacter::Interact()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_Interaction);

	if (IsValid(TargetInteractable.GetObject()))
	{
		TargetInteractable->Interact(this);
	}
}

void AVICTIMSCharacter::OnBeginOverlapInteractableRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	InteractionData.LastInteractionCheckTime = GetWorld()->GetTimeSeconds();

	if (OtherActor->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
	{
		if (OtherActor != InteractionData.CurrentInteractable)
		{	// 오버랩된 액터중 상호작용 가능한 액터가 있다면 상호작용 가능한 액터 찾음 함수 호출
			FoundInteractable(OtherActor);
			return;
		}
		if (OtherActor == InteractionData.CurrentInteractable)
		{
			return;
		}

	}
	NoInteractableFound();
}

void AVICTIMSCharacter::OnEndOverlapInteractableRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// 상호작용 안되게 어떻게 해야하지........ 흠 
	GetWorldTimerManager().ClearTimer(TimerHandle_Interaction);

	if (IsValid(TargetInteractable.GetObject()))
	{
		TargetInteractable->EndInteract();
	}
}
void AVICTIMSCharacter::ToggleMenu()
{
	
}
void AVICTIMSCharacter::FoundInteractable(AActor* NewInteractable)
{
	if (IsInteracting())
	{
		EndInteract();
	}
	if (InteractionData.CurrentInteractable)
	{
		TargetInteractable = InteractionData.CurrentInteractable;
		TargetInteractable->EndFocus();
	}
	InteractionData.CurrentInteractable = NewInteractable;
	TargetInteractable = NewInteractable;

	HUD->UpdateInteractionWidget(&TargetInteractable->InteractableData);
	TargetInteractable->BeginFocus();
}

void AVICTIMSCharacter::NoInteractableFound()
{
	if (IsInteracting())
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_Interaction);
	}
	if (InteractionData.CurrentInteractable)
	{
		if (IsValid(TargetInteractable.GetObject()))
		{
			TargetInteractable->EndFocus();
			EndInteract();
		}
		HUD->HideInteractionWidget();
		InteractionData.CurrentInteractable = nullptr;
		TargetInteractable = nullptr;
	}
}

void AVICTIMSCharacter::BeginInteract()
{
	if (InteractionData.CurrentInteractable)
	{
		if (IsValid(TargetInteractable.GetObject()))
		{
			TargetInteractable->BeginInteract();

			if (FMath::IsNearlyZero(TargetInteractable->InteractableData.InteractionDuration, 0.1f))
			{
				Interact();
			}
			else
			{
				GetWorldTimerManager().SetTimer(TimerHandle_Interaction,
					this,
					&AVICTIMSCharacter::Interact,
					TargetInteractable->InteractableData.InteractionDuration,
					false);
			}
		}
	}
}

void AVICTIMSCharacter::EndInteract()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_Interaction);

	if (IsValid(TargetInteractable.GetObject()))
	{
		TargetInteractable->EndInteract();
	}
}
void AVICTIMSCharacter::UpdateInteractionWidget() const
{
	if (IsValid(TargetInteractable.GetObject()))
	{
		HUD->UpdateInteractionWidget(&TargetInteractable->InteractableData);
	}
}

void AVICTIMSCharacter::DropItem(UItemBase* ItemToDrop, const int32 QuantityToDrop)
{
	if (PlayerInventory->FindMatchingItem(ItemToDrop))
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.bNoFail = true;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		const FVector SpawnLocation{ GetActorLocation() + (GetActorForwardVector() * 50.0f) };
		const FTransform SpawnTransform(GetActorRotation(), SpawnLocation);

		const int32 RemovedQuantity = PlayerInventory->RemoveAmountOfItem(ItemToDrop, QuantityToDrop);

		APickUp* Pickup = GetWorld()->SpawnActor<APickUp>(APickUp::StaticClass(), SpawnTransform, SpawnParams);

		Pickup->InitializeDrop(ItemToDrop, RemovedQuantity);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Item to drop was somehow null!"));
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

	if (IsLocallyControlled())
	{
		auto pc = Cast<APlayerController>(Controller);

		if (pc)
		{
			DisableInput(pc);
		}
	}

	motionState = ECharacterMotionState::Die;

	Super::DieFunction();
}


void AVICTIMSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

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
	auto mainWeaponPtr = combatComponent->GetMainWeapon();
	if (IsValid(mainWeaponPtr))
	{
		if (motionState == ECharacterMotionState::Idle)
		{
			UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("combatComponent->bCombatEnable : %s"), combatComponent->bCombatEnable ? TEXT("TRUE") : TEXT("FALSE")));

			ServerRPC_ToggleCombat();
		}
	}
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

void AVICTIMSCharacter::ServerRPC_ToggleCombat_Implementation()
{
	motionState = ECharacterMotionState::ToggleCombat;

	combatComponent->bCombatEnable = !combatComponent->bCombatEnable;

	NetMulticastRPC_ToggleCombat();
}

void AVICTIMSCharacter::NetMulticastRPC_ToggleCombat_Implementation()
{
	auto mainWeaponPtr = combatComponent->GetMainWeapon();

	float animPlayTime = 0.0f;

	if (!combatComponent->bCombatEnable)
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

	//FString netMode = UEnum::GetValueAsString(GetNetMode());

	FString hasController = Controller ? TEXT("HasController") : TEXT("NoController");

	FString strHP = FString::Printf(TEXT("%f"), stateComp->GetStatePoint(EStateType::HP));
	FString strSP = FString::Printf(TEXT("%f"), stateComp->GetStatePoint(EStateType::SP));

	//FString str = FString::Printf(TEXT("localRole : %s\nremoteRole : %s\nowner : %s\nnetConn : %s\nnetMode : %s\nhasController : %s\n HP : %s\n SP : %s"), *localRole, *remoteRole, *owner, *netConn, /**netMode,*/ *hasController, *strHP, *strSP);

	FString str = FString::Printf(TEXT("localRole : %s\nremoteRole : %s\nowner : %s\nnetConn : %s\nhasController : %s\n HP : %s\n SP : %s"), *localRole, *remoteRole, *owner, *netConn, *hasController, *strHP, *strSP);

	FVector loc = GetActorLocation() + FVector(0, 0, 50);
	DrawDebugString(GetWorld(), loc, str, nullptr, FColor::White, 0, true);
}