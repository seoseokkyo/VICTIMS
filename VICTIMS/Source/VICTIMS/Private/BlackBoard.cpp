// Fill out your copyright notice in the Description page of Project Settings.


#include "BlackBoard.h"
#include "Camera/CameraComponent.h"
#include "AVICTIMSPlayerController.h"
#include "VICTIMSCharacter.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ABlackBoard::ABlackBoard()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	ViewCamera->SetupAttachment(RootComponent); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation

	ViewCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	ViewCamera->SetRelativeLocation(FVector(45.0f, 150.0f, -40.0f));
	ViewCamera->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));



	IsUsable = true;
	Action = FText::FromString("Travel");
	Name = FText::FromString("Target Place");
}

// Called when the game starts or when spawned
void ABlackBoard::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ABlackBoard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ABlackBoard::OnActorUsed_Implementation(APlayerController* Controller)
{
	if (bUsingNow)
	{
		return true;
	}

	useingPlayer = Cast<AVICTIMSPlayerController>(Controller);

	if (nullptr != useingPlayer)
	{
		SetOwner(useingPlayer);

		TArray<AActor*> arrayFoundPictures;
		arrayFoundPictures.Reset();

		GetAllChildActors(arrayFoundPictures);

		for (auto find : arrayFoundPictures)
		{
			find->SetOwner(useingPlayer);

			// ??????
			//if (find->Tags.Contains(TEXT("Picture")))
			//{
			//	find->SetOwner(useingPlayer);
			//}
		}
		
		if (ViewCamera)
		{
			useingPlayer->ServerRPC_SetUseUIState(true);

			ServerRPC_TravelRequest(useingPlayer);

			return false;
		}

		return true;
	}

	return false;
}

void ABlackBoard::ServerRPC_TravelRequest_Implementation(AVICTIMSPlayerController* Controller)
{
	auto charCheck = Controller->GetPawn();

	charCheck->DisableInput(Controller);

	FVector tempLocation = GetActorRightVector() * 300 + GetActorLocation();
	//charCheck->SetActorLocation(tempLocation, false, nullptr, ETeleportType::TeleportPhysics);
	//charCheck->TeleportTo(charCheck->GetActorLocation(), charCheck->GetActorRotation());;

	this->EnableInput(Controller);

	Controller->ServerRPC_SetUseUIState(true);

	useingPlayer = Controller;

	ClientRPC_TravelRequest(Controller, Cast<AVICTIMSCharacter>(charCheck), tempLocation);
}

void ABlackBoard::ClientRPC_TravelRequest_Implementation(AVICTIMSPlayerController* Controller, AVICTIMSCharacter* playerCharacter, FVector loc)
{
	if (Controller == nullptr || playerCharacter == nullptr)
	{
		return;
	}

	//playerCharacter->DisableInput(Controller);
	playerCharacter->SetActorLocation(loc, false, nullptr, ETeleportType::TeleportPhysics);
	//playerCharacter->TeleportTo(playerCharacter->GetActorLocation(), playerCharacter->GetActorRotation());;

	playerCharacter->DisableInput(Controller);

	this->EnableInput(Controller);

	float blendTime = 0.4f;

	Controller->SetViewTargetWithBlend(this, blendTime, VTBlend_Linear);

	FTimerHandle timerHnd;

	GetWorldTimerManager().SetTimer(timerHnd, [&, Controller]() {

		ServerRPC_SetUIMode(Controller);
		Controller->ServerRPC_SetUseUIState(true);
		Controller->bUseUIMode = true;
		Controller->EnableUIMode();
		Controller->SetShowMouseCursor(true);
		Controller->bEnableClickEvents = true;
		Controller->bEnableMouseOverEvents = true;

		}, blendTime + 0.3f, false);
}

void ABlackBoard::ServerRPC_SetUIMode_Implementation(AVICTIMSPlayerController* Controller)
{
	useingPlayer->bUseUIMode = true;
	useingPlayer->EnableUIMode();
	useingPlayer->SetShowMouseCursor(true);
	useingPlayer->bEnableClickEvents = true;
	useingPlayer->bEnableMouseOverEvents = true;
}

void ABlackBoard::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlackBoard, bUsingNow);
}
