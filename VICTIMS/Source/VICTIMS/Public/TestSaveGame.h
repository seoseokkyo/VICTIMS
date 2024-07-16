
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "TestSaveGame.generated.h"


USTRUCT()
struct FPlayerSaveData
{
	GENERATED_BODY()
public:

	FPlayerSaveData()					// Struct �����Ͱ� �ʱ�ȭ 
	{
		HP = 0; 
		Gold = 0;
		Location = FVector::ZeroVector;
		Rotation = FRotator::ZeroRotator;
	}

	UPROPERTY()
	FString PlayerID;

	UPROPERTY()
	float HP;
	UPROPERTY()
	uint8 Gold;
}

UCLASS()
class VICTIMS_API UTestSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:

	FPlayerSaveData PlayerDataStructure;
};
