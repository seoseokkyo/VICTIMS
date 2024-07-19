// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PackedLevelParent.h"
#include "Shelter.generated.h"

/**
 * 
 */
UCLASS()
class VICTIMS_API AShelter : public APackedLevelParent
{
	GENERATED_BODY()
	
public:

	//AShelter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
	FString OwnerPlayerID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shelter")
	int32 HouseNumber;
};
