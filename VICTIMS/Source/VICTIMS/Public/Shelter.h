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

	UPROPERTY(Replicated ,VisibleAnywhere, BlueprintReadWrite, Category = "Shelter")
	FString OwnerPlayerName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shelter")
	int32 HouseNumber;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Shelter")
	FVector OriginPos;

	UFUNCTION(Server, Reliable)
	void ServerRPC_SetOriginPos(FVector houseLocation);

	UFUNCTION()
	void SetPlayerName(FString ID);

	UFUNCTION(Server, Reliable)
	void ServerRPC_SetPlayerName(const FString& ID);

	UFUNCTION(Netmulticast, Reliable)
	void NetMulticastRPC_SetPlayerName(const FString& ID);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
