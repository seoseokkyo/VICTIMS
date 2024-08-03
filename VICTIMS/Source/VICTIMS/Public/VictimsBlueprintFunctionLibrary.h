// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VictimsBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class VICTIMS_API UVictimsBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "FileIO")
	static bool WriteStringToFile(const FString& FileName, const FString& TextToWrite);

	UFUNCTION(BlueprintCallable, Category = "FileIO")
	static bool ReadStringFromFile(const FString& FileName, FString& OutText);
	
};
