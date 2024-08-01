// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UsableActor.h"
#include "TravelActor.generated.h"

UCLASS()
class VICTIMS_API ATravelActor : public AUsableActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATravelActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(Server, Reliable)
	void ServerRPC_TravelRequest(APlayerController* Controller);
	
	virtual bool OnActorUsed_Implementation(APlayerController* Controller) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString strPort;
};
