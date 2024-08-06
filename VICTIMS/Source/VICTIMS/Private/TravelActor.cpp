// Fill out your copyright notice in the Description page of Project Settings.


#include "TravelActor.h"
#include "VictimsGameInstance.h"
#include "AVICTIMSPlayerController.h"
#include "VICTIMSCharacter.h"
#include "StateComponent.h"

// Sets default values
ATravelActor::ATravelActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 정커타운 포트번호
	strPort = TEXT("8101");

	Name = FText::FromString("Home");
	Action = FText::FromString("Travel");
}

// Called when the game starts or when spawned
void ATravelActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATravelActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATravelActor::ServerRPC_TravelRequest_Implementation(APlayerController* Controller)
{
	if (auto gi = Cast<UVictimsGameInstance>(GetGameInstance()))
	{
		FString URL = gi->mainAddress + TEXT(":") + strPort;

		if (auto playerctrl = Cast<AVICTIMSPlayerController>(Controller))
		{
			playerctrl->RequestClientTravel(URL, FString());
		}
	}
}

bool ATravelActor::OnActorUsed_Implementation(APlayerController* Controller)
{
	SetOwner(Controller);

	Super::OnActorUsed_Implementation(Controller);

	ServerRPC_TravelRequest(Controller);

	return false;
}

