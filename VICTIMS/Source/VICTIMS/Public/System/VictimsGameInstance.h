// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Engine/DataTable.h"
#include "VictimsGameInstance.generated.h"

/**
 * 
 */

class UDataTable;

UCLASS()
class VICTIMS_API UVictimsGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
	FCharacterStat GetCharacterDataTable(const FString& rowName);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ConnectToServerAndMoveToNewLevel(APlayerController* PlayerController, const FString& NewLevelName);
	

	UFUNCTION()
	void StreamingLevelTest();

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void LoadSubLevel(APlayerController* PlayerController, const FString& LevelName);


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MySettings")
	UDataTable* dt_characerStatDataTable;
};
