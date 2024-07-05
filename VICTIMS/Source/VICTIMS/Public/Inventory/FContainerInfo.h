﻿#pragma once

#include "CoreMinimal.h"
#include "FContainerInfo.generated.h"

USTRUCT(BlueprintType)
struct VICTIMS_API FContainerInfo
{
	GENERATED_BODY();

	FContainerInfo();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	FName ContainerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	uint8 NumberOfRows;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	uint8 SlotsPerRow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	bool IsStorageContainer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	uint8 StorageInventorySize;
};