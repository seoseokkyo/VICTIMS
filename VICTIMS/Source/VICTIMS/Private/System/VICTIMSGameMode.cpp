// Copyright Epic Games, Inc. All Rights Reserved.

#include "VICTIMSGameMode.h"
#include "VICTIMSCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "AVICTIMSPlayerController.h"
#include "VICTIMSCharacter.h"
#include "BaseWeapon.h"
#include <../../../../../../../Source/Runtime/Engine/Public/EngineUtils.h>
#include "GameFramework/PlayerState.h"
#include "Shelter.h"
#include "TestSaveGame.h"

AVICTIMSGameMode::AVICTIMSGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	TotalHouses = 0;
	NextHouseIndex = 0;
}


void AVICTIMSGameMode::BeginPlay()
{
	Super::BeginPlay();

	int32 HouseIndex = 0;
	for (TActorIterator<AShelter> it(GetWorld()); it; ++it)
	{
		if (*it != nullptr)
		{
			Houses.Add(*it);
			(*it)->HouseNumber = HouseIndex++;
		}
	}

	TotalHouses = Houses.Num();

	for (FConstPlayerControllerIterator IT = GetWorld()->GetPlayerControllerIterator(); IT; ++IT)
	{
		APlayerController* pc = IT->Get();
		if (pc != nullptr)
		{
			AssignHouseToPlayer(Cast<AVICTIMSPlayerController>(pc));
		}
	}
}

void AVICTIMSGameMode::PostLogin(APlayerController* pc)
{
	Super::PostLogin(pc);

	if (HasAuthority())
	{
		AVICTIMSPlayerController* NewPlayer = Cast<AVICTIMSPlayerController>(pc);
		if (NewPlayer)
		{
			FString PlayerID = FString::FromInt(NewPlayer->PlayerState->GetPlayerId());
			UTestSaveGame* LoadedData = NewPlayer->GetSaveDataFromID(PlayerID);
			if (LoadedData && LoadedData->HouseNumber >= 0 && LoadedData->HouseNumber < Houses.Num())
			{
				AShelter* AssignedHouse = Houses[LoadedData->HouseNumber];
				AVICTIMSCharacter* PlayerCharacter = Cast<AVICTIMSCharacter>(NewPlayer->GetPawn());
				if (PlayerCharacter)
				{
					PlayerCharacter->SetAssignedHouse(AssignedHouse);
				}
			}
			else
			{
				AssignHouseToPlayer(NewPlayer);
			}
		}
	}
}

void AVICTIMSGameMode::AssignHouseToPlayer(AVICTIMSPlayerController* NewPlayer)
{
	if (HasAuthority())
	{
		if (NewPlayer == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("AssignHouseToPlayer: NewPlayer is null"));
			return;
		}

		if (NextHouseIndex >= TotalHouses)
		{
			UE_LOG(LogTemp, Warning, TEXT("AssignHouseToPlayer: No houses available"));
			return;
		}

		AVICTIMSCharacter* PlayerCharacter = Cast<AVICTIMSCharacter>(NewPlayer->GetPawn());
		if (PlayerCharacter == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("AssignHouseToPlayer: PlayerCharacter is null"));
			return;
		}

		if (NextHouseIndex < TotalHouses)
		{
			//AVICTIMSCharacter* PlayerCharacter = Cast<AVICTIMSCharacter>(NewPlayer->GetPawn());
			//FString PlayerID = NewPlayer->PlayerState->GetPlayerId();
			int32 PlayerIDInt = NewPlayer->PlayerState->GetPlayerId();
			FString PlayerID = FString::FromInt(PlayerIDInt);
			AShelter* AssignedHouse = Houses[NextHouseIndex];
			if (AssignedHouse == nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("AssignHouseToPlayer: AssignedHouse is null"));
				return;
			}
			AssignedHouse->OwnerPlayerID = PlayerID;

			if (PlayerCharacter)
			{
				PlayerCharacter->SetAssignedHouse(AssignedHouse);
			}

			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Player %d assigned to House %d"), PlayerIDInt, NextHouseIndex));
			}

			if (NewPlayer->SavedData != nullptr)
			{
				NewPlayer->SavedData->HouseNumber = NextHouseIndex;
				NewPlayer->SaveData(PlayerID);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("AssignHouseToPlayer: NewPlayer->SavedData is null"));
			}

			NextHouseIndex++;
			//if (PlayerCharacter)
			//{
			//	APackedLevelParent* AssignedHouse = Houses[HouseIndex];
			//	AssignedHouse->OwnerPlayerID = PlayerCharacter->PlayerID;
			//	
			//}
		}

		// 	static int32 HouseIndex = 0;
		// 	if (HouseIndex < TotalHouses)
		// 	{
		//	}
	}
}


void AVICTIMSGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	if (APlayerController* PlayerController = Cast<APlayerController>(Exiting))
	{
		if (AVICTIMSPlayerController* victimsPlayerController = Cast<AVICTIMSPlayerController>(PlayerController))
		{
			if (victimsPlayerController != nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("Player %s has logged out"), *victimsPlayerController->GetActorNameOrLabel());

				auto playerCheck = Cast<AVICTIMSCharacter>(victimsPlayerController->GetPawn());
				//if (playerCheck != nullptr)
				//{
				//	playerCheck->defaultWeapon->BeginDestroy();
				//}
			}
		}
	}
}
