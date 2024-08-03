// Fill out your copyright notice in the Description page of Project Settings.


#include "VictimsBlueprintFunctionLibrary.h"




bool UVictimsBlueprintFunctionLibrary::WriteStringToFile(const FString& FileName, const FString& TextToWrite)
{
    return FFileHelper::SaveStringToFile(TextToWrite, *FileName);
}

bool UVictimsBlueprintFunctionLibrary::ReadStringFromFile(const FString& FileName, FString& OutText)
{
    return FFileHelper::LoadFileToString(OutText, *FileName);
}