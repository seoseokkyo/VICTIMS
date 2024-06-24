// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlackjackCard.generated.h"


UENUM()
enum class ECardType : int8
{
	Clover = 0,
	Heart,
	Diamond,
	Spade,
	ECardTypeMax
};


USTRUCT(BlueprintType)
struct FBlackjackCardInfo : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECardType cardType;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//char cardValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString cardValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMesh* cardMesh;
};


UCLASS()
class VICTIMS_API ABlackjackCard : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABlackjackCard();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* CardMeshComp;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FBlackjackCardInfo cardInfo;
	
};
