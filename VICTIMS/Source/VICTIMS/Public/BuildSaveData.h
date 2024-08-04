// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "BuildSaveData.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FBuildSaveDataStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ID_Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ID_Num;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//FTransform transform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector location;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator rotation;
};


UCLASS()
class VICTIMS_API UBuildSaveData : public USaveGame
{
	GENERATED_BODY()

public:

	UPROPERTY()
	TArray<FBuildSaveDataStruct> buildDatas;
};
