// Copyright Epic Games, Inc. All Rights Reserved.

#include "VICTIMSGameMode.h"
#include "VICTIMSCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "AVICTIMSPlayerController.h"
#include "VICTIMSCharacter.h"
#include "BaseWeapon.h"
#include <../../../../../../../Source/Runtime/Engine/Public/EngineUtils.h>
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h>
#include "GameFramework/PlayerState.h"
#include "Shelter.h"
#include "TestSaveGame.h"
#include "Net/UnrealNetwork.h"

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

			auto OriginPos = (*it)->GetComponentsByTag(USceneComponent::StaticClass(), FName(TEXT("OriginPos")));

			for (auto temp : OriginPos)
			{
				(*it)->ServerRPC_SetOriginPos(Cast<USceneComponent>(temp)->GetComponentLocation());

				UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("House OriginPos X : %f, Y : %f, Z : %f"), (*it)->OriginPos.X, (*it)->OriginPos.Y, (*it)->OriginPos.Z));
			}
		}
	}

	TotalHouses = Houses.Num();

	//for (FConstPlayerControllerIterator IT = GetWorld()->GetPlayerControllerIterator(); IT; ++IT)
	//{
	//	APlayerController* pc = IT->Get();
	//	if (pc != nullptr)
	//	{
	//		AssignHouseToPlayer(Cast<AVICTIMSPlayerController>(pc));
	//	}
	//}
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

			AVICTIMSCharacter* PlayerCharacter = Cast<AVICTIMSCharacter>(NewPlayer->GetPawn());
			if (PlayerCharacter)
			{
				PlayerCharacter->MyPlayerController = NewPlayer;
			}

			AssignHouseToPlayer(NewPlayer);

			//UTestSaveGame* LoadedData = NewPlayer->GetSaveDataFromID(PlayerID);

			//if (LoadedData && LoadedData->HouseNumber >= 0 && LoadedData->HouseNumber < Houses.Num())

			//if (LoadedData && LoadedData->HouseNumber < Houses.Num())
			//{
			//	//AShelter* AssignedHouse = Houses[LoadedData->HouseNumber];
			//	AShelter* AssignedHouse = FindUnOwnedHouse();

			//	AVICTIMSCharacter* PlayerCharacter = Cast<AVICTIMSCharacter>(NewPlayer->GetPawn());
			//	if (PlayerCharacter)
			//	{
			//		PlayerCharacter->MyPlayerController = NewPlayer;
			//		PlayerCharacter->SetAssignedHouse(AssignedHouse);

			//		AssignHouseToPlayer(NewPlayer);
			//	}
			//}
			//else
			//{

			//}
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

		//AVICTIMSCharacter* PlayerCharacter = Cast<AVICTIMSCharacter>(NewPlayer->GetPawn());
//FString PlayerID = NewPlayer->PlayerState->GetPlayerId();
		int32 PlayerIDInt = NewPlayer->PlayerState->GetPlayerId();
		FString PlayerID = FString::FromInt(PlayerIDInt);

		//AShelter* AssignedHouse = Houses[NextHouseIndex];

		AShelter* AssignedHouse = FindUnOwnedHouse();

		if (AssignedHouse == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("AssignHouseToPlayer: AssignedHouse is null"));

			AssignedHouse = Houses[0];
		}

		AssignedHouse->Ownerplayer = NewPlayer;
		AssignedHouse->OwnerPlayerID = PlayerID;

		if (PlayerCharacter)
		{
			PlayerCharacter->SetAssignedHouse(AssignedHouse);
		}

		//if (GEngine)
		//{
		//	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Player %d assigned to House %d"), PlayerIDInt, NextHouseIndex));
		//}

		//if (auto savedData = NewPlayer->GetSaveDataFromID(PlayerID))
		//{
		//	savedData->HouseNumber = NextHouseIndex;
		//	NewPlayer->SaveData();
		//}
		//else
		//{
		//	UE_LOG(LogTemp, Warning, TEXT("AssignHouseToPlayer: NewPlayer->SavedData is null"));
		//}

		//NextHouseIndex++;

		//if (NextHouseIndex < TotalHouses)
		//{

		//}
	}
}

EVictimsNetMode AVICTIMSGameMode::GetGameNetMode()
{
	return EVictimsNetMode(GetNetMode());
}

void AVICTIMSGameMode::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AVICTIMSGameMode, Houses);
}

TArray<AVICTIMSPlayerController*> AVICTIMSGameMode::GetPlayers()
{
	TArray<AVICTIMSPlayerController*> players;
	players.Empty();

	for (auto it(GetWorld()->GetPlayerControllerIterator()); it; ++it)
	{
		auto playerCheck = Cast<AVICTIMSPlayerController>(*it);

		if (playerCheck)
		{
			players.Add(playerCheck);
		}
	}

	return players;
}

AShelter* AVICTIMSGameMode::FindUnOwnedHouse()
{
	AShelter* shelter = nullptr;

	if (HasAuthority())
	{

		for (const auto& house : Houses)
		{
			if (house->OwnerPlayerName.IsEmpty())
			{
				shelter = house;
				UE_LOG(LogTemp, Warning, TEXT("FindUnOwnedHouse : %s"), *shelter->GetActorNameOrLabel());

				break;
			}
		}
	}

	return shelter;
}

void AVICTIMSGameMode::ClearHouseOwnership(FString strPlayerName)
{
	for (auto& house : Houses)
	{
		if (house->OwnerPlayerName == strPlayerName)
		{
			UE_LOG(LogTemp, Warning, TEXT("Check : Shelter Owner Is %s"), *house->OwnerPlayerName);

			house->Ownerplayer = nullptr;
			house->OwnerPlayerID = FString();
			house->OwnerPlayerName = FString();

			UE_LOG(LogTemp, Warning, TEXT("Clear : Shelter Owner Is %s"), *house->OwnerPlayerName);
		}
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

				ClearHouseOwnership(victimsPlayerController->playerName);
				//if (playerCheck != nullptr)
				//{
				//	playerCheck->defaultWeapon->BeginDestroy();
				//}
			}
		}
	}
}
