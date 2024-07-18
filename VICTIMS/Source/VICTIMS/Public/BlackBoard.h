// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item/UsableActor.h"
#include "BlackBoard.generated.h"


class UCameraComponent;
class AVICTIMSPlayerController;

UCLASS()
class VICTIMS_API ABlackBoard : public AUsableActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABlackBoard();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCameraComponent* ViewCamera;	

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AVICTIMSPlayerController* useingPlayer;

	UPROPERTY(BlueprintReadWrite)
	bool bUsingNow = false;

	virtual bool OnActorUsed_Implementation(APlayerController* Controller) override;

	UFUNCTION(Server, Reliable)
	void ServerRPC_TravelRequest(AVICTIMSPlayerController* Controller);

	UFUNCTION(MultiRPC, Reliable)
	void MultiRPC_TravelRequest(AVICTIMSPlayerController* Controller);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
};
