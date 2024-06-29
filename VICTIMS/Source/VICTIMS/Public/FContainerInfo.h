
#pragma once

#include "CoreMinimal.h"
#include "FContainerInfo.generated.h"

USTRUCT(BlueprintType)
struct VICTIMS_API FContainerInfo
{
	GENERATED_BODY();

public:

	FContainerInfo();		// 보관함 이름, 저장공간, 보관함 판별여부 설정 
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FName containerName; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	uint8 numberOfRows;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	uint8 slotsPerRow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	bool bIsContainer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	uint8 storageSize;
};
