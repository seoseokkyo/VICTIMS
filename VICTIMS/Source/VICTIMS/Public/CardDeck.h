// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <vector>
#include "Engine/DataTable.h"
#include "CardDeck.generated.h"

UCLASS()
class VICTIMS_API ACardDeck : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACardDeck();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Ace = 1, J = 10, Q = 10, K = 10;

	//Clover, Heart, Diamond, Spade
	//Ace, 2, 3, 4, 5, 6, 7, 8, 9, 10, J, Q, K
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class ABlackjackCard*> CardSet;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//std::vector<std::vector<int>> CardSet;

	TArray<struct FBlackjackCardInfo*> CardInfos;

	UFUNCTION(BlueprintCallable)
	void init_deck();

	UFUNCTION(BlueprintCallable)
	void shuffler();

	UFUNCTION(BlueprintCallable)
	void print_deck();

	UFUNCTION(BlueprintCallable)
	ABlackjackCard* getCard();

	UFUNCTION(BlueprintCallable)
	bool is_deck_empty();

	UFUNCTION(BlueprintCallable)
	void ResetDeck();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<class ABlackjackCard> BP_CardFactory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* dt_cardInfoDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsEmpty=false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
