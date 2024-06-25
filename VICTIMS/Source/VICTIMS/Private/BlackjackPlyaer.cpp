// Fill out your copyright notice in the Description page of Project Settings.


#include "BlackjackPlyaer.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/Camera/CameraComponent.h>
#include <../../../../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputSubsystems.h>
#include <../../../../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputComponent.h>

// Sets default values
ABlackjackPlyaer::ABlackjackPlyaer()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	//FollowCamera->SetupAttachment(RootComponent);
	FollowCamera->SetRelativeLocation(FVector(0, 0, 150) + GetActorLocation());
}

// Called when the game starts or when spawned
void ABlackjackPlyaer::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ABlackjackPlyaer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABlackjackPlyaer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//// Add Input Mapping Context
	//if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	//{
	//	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	//	{
	//		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	//	}
	//}
	//
	//// Set up action bindings
	//if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) 
	//{
	//	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABlackjackPlyaer::Look);
	//}
	//// Looking
}


void ABlackjackPlyaer::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	FollowCamera->AddRelativeRotation(FRotator(-LookAxisVector.Y, LookAxisVector.X, 0));


	//if (Controller != nullptr)
	//{
	//	// add yaw and pitch input to controller
	//	AddControllerYawInput(LookAxisVector.X);
	//	AddControllerPitchInput(LookAxisVector.Y);
	//}
}