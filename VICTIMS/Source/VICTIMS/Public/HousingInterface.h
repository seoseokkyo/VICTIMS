// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HousingInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UHousingInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class VICTIMS_API IHousingInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	TArray<class UBoxComponent*> ReturnBoxes();
	TArray<class UBoxComponent*> ReturnBoxes_Implementation() PURE_VIRTUAL(IHousingInterface::ReturnBoxes_Implementation, return TArray<class UBoxComponent*>(););

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetMesh(UStaticMesh* Mesh);
	void SetMesh_Implementation(UStaticMesh* Mesh) PURE_VIRTUAL(IHousingInterface::SetMesh_Implementation, return;);
};
