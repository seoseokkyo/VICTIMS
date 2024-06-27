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
#include "HousingComponent.h"

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

	interactionCheckFrequency = 0.1f;
	interactionCheckDistance = 225.0f;

	characterName = TEXT("Player");


	HousingComponent = CreateDefaultSubobject<UHousingComponent>(TEXT("HousingComp"));
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AVICTIMSCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	mainHUD = Cast<AMainHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());

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

	if (HousingComponent)
	{
		HousingComponent->Camera = FollowCamera; // 초기화 시 카메라 컴포넌트를 할당
	}
}


void AVICTIMSCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// 상호작용 가능 액터 찾기 
	if (GetWorld()->TimeSince(interactionData.lastInteractionCheckTime) > interactionCheckFrequency)
	{
		PerformInteractionCheck();
	}
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

		EnhancedInputComponent->BindAction(ia_Interact, ETriggerEvent::Triggered, this, &AVICTIMSCharacter::BeginInteract);
		EnhancedInputComponent->BindAction(ia_Interact, ETriggerEvent::Completed, this, &AVICTIMSCharacter::EndInteract);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AVICTIMSCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AVICTIMSCharacter::Look);
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

void AVICTIMSCharacter::PerformInteractionCheck()
{
	interactionData.lastInteractionCheckTime = GetWorld()->GetTimeSeconds();

	FVector start = GetPawnViewLocation();
	FVector end = start + (GetViewRotation().Vector() * interactionCheckDistance);


	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(this);
	FHitResult result;

	float lookDir = FVector::DotProduct(GetActorForwardVector(), GetViewRotation().Vector());

	if (lookDir > 0)
	{

	// 	DrawDebugLine(GetWorld(), start, end, FColor::Red, false, 1.0f, 0, 2.0f);
		// 라인트레이스 (카메라뷰 기준) 에 검출된 액터가 있을 때 
		if (GetWorld()->LineTraceSingleByChannel(result, start, end, ECC_Visibility, queryParams))
		{
			if (result.GetActor()->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
			{
				if (result.GetActor() != interactionData.currentInteractable)
				{
					// 상호작용 가능 + 상호작용 가능거리 일 때, 그 액터와 상호작용 준비 
					FoundInteractable(result.GetActor());
					return;
				}
					// 이미 상호작용 가능 액터를 찾은 상태라면(위 조건에 들어가 있는 상태) 더이상 검출시도 X
				if (result.GetActor() == interactionData.currentInteractable)
				{
					return;
				}
			}
		}
	}
	NoInteractableFound();
}

void AVICTIMSCharacter::FoundInteractable(AActor* newInteractable)
{
	if (IsInteracting())
	{
		EndInteract();
	}
	if (interactionData.currentInteractable)
	{
		targetInteractable = interactionData.currentInteractable;
		targetInteractable->EndFocus();
	}
	interactionData.currentInteractable = newInteractable;
	targetInteractable = newInteractable;

	mainHUD->UpdateInteractionWidget(&targetInteractable->interactableData);

	targetInteractable->BeginFocus();
}

void AVICTIMSCharacter::NoInteractableFound()
{
	if (IsInteracting())
	{
		GetWorldTimerManager().ClearTimer(timerHandle_Interaction);
	}
	if (interactionData.currentInteractable)
	{
		if (IsValid(targetInteractable.GetObject()))
		{
			targetInteractable->EndFocus();
		}

		mainHUD->HideInteractionWidget();

		// 상호작용 UI 숨기기 
		interactionData.currentInteractable = nullptr;
		targetInteractable = nullptr;
	}
}

void AVICTIMSCharacter::BeginInteract()
{
	// 상호작용 키 눌렀을 때 
	// 상호작용 시작 전 다시 한번 상호작용 가능한 조건인지 확인
	PerformInteractionCheck();
	if (interactionData.currentInteractable)
	{
		if (IsValid(targetInteractable.GetObject()))
		{
			targetInteractable->BeginInteract();
			if (FMath::IsNearlyZero(targetInteractable->interactableData.interactionDuration, 0.1f))
			{	
				// 바로 상호작용 
				Interact();
			}
			else 
			{
				// 상호작용 걸리는 시간 이후에 상호작용 
				GetWorldTimerManager().SetTimer(timerHandle_Interaction,
					this, &AVICTIMSCharacter::Interact,
					targetInteractable->interactableData.interactionDuration, false);
			}
		}
	}
}

void AVICTIMSCharacter::EndInteract()
{
	// 상호작용 키 눌렀다 뗐을 때 
	GetWorldTimerManager().ClearTimer(timerHandle_Interaction);
	if (IsValid(targetInteractable.GetObject()))
	{
		targetInteractable->EndInteract();
	}
}

void AVICTIMSCharacter::Interact()
{
	// 실제 상호작용 
	GetWorldTimerManager().ClearTimer(timerHandle_Interaction);
	if (IsValid(targetInteractable.GetObject()))
	{
		targetInteractable->Interact(this);
	}
}

float AVICTIMSCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	return 0.0f;
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

			UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("animPlayTime : %f"), animPlayTime));
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

			UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("animPlayTime : %f"), animPlayTime));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ToggleCombatFunction : %d"), __LINE__);
		}
	}

	FTimerHandle timerHandle;

	GetWorldTimerManager().SetTimer(timerHandle, [&]()
		{
			motionState = ECharacterMotionState::Idle;

			GetWorld()->GetTimerManager().ClearTimer(timerHandle);

			UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("combatComponent->bCombatEnable : %s"), combatComponent->bCombatEnable ? TEXT("TRUE") : TEXT("FALSE")));

		}, animPlayTime, false, 1.0f);
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


void AVICTIMSCharacter::DestroyComponent(UActorComponent* TargetComponent)
{
	if (TargetComponent)
	{
		TargetComponent->DestroyComponent();
	}
}
