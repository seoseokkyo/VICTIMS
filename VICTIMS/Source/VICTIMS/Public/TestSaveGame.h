
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "TestSaveGame.generated.h"


USTRUCT()
struct FPlayerSaveData
{
	GENERATED_BODY()
public:

	UPROPERTY()							// Save ÆÄÀÏ ID 
	FString PlayerID;

	UPROPERTY()
	float HP;
	UPROPERTY()
	uint8 Gold;
};

UCLASS()
class VICTIMS_API UTestSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:

	UTestSaveGame();

	FPlayerSaveData PlayerDataStructure;

	UPROPERTY()
	float SavedHP;
	UPROPERTY()
	uint8 SavedGold;

	UPROPERTY()
	FString ItemID;

	UPROPERTY()
	uint8 SavedItemAmount;

	UPROPERTY()
	uint8 SavedItemSlot;

	UPROPERTY()
	TArray<uint8> SavedItemSlots;

	UPROPERTY()
	TArray<uint8> SavedItemAmounts;
};
