
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "TestSaveGame.generated.h"


USTRUCT()
struct FPlayerSaveData
{
	GENERATED_BODY()
public:

	UPROPERTY()
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

	FPlayerSaveData PlayerDataStructure;
};
