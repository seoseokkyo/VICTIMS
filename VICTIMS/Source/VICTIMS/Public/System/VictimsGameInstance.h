// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Engine/DataTable.h"
#include "VictimsGameInstance.generated.h"

/**
 *
 */

USTRUCT()
struct FInstantServerStruct
{
	GENERATED_BODY()

	int32	serverNumber;
	int32	serverPortNumber;
	int32	playerNum;
	FString serverLevelName;

	TArray<class APlayerController*> clients;
};


class UDataTable;

UCLASS()
class VICTIMS_API UVictimsGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	FCharacterStat GetCharacterDataTable(const FString& rowName);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ConnectToServerAndMoveToNewLevel(APlayerController* PlayerController, const FString& NewLevelName, ETravelType type);

	UFUNCTION()
	virtual void OnStart() override;

	UPROPERTY()
	FString serverType;

	UPROPERTY()
	FString serverPort;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MySettings")
	UDataTable* dt_characerStatDataTable;

	UFUNCTION(Server, Unreliable, BlueprintCallable)
	void ServerRPC_PrintServerType();

	UFUNCTION(NetMulticast, Unreliable, BlueprintCallable)
	void MultiRPC_PrintServerType(const FString& serverTypeString);

	UPROPERTY()
	TArray<FInstantServerStruct> InstantServerArray;

	UPROPERTY()
	class UProcSocketObject* mySocket;

	UPROPERTY()
	TArray<FTimerHandle> clientTravelTimers;

	UFUNCTION()
	void TryToClientTravel(int waitIndex);

	UFUNCTION()
	virtual void ShutDown();

	UPROPERTY()
	class UTestSaveGame* SaveData;
};
