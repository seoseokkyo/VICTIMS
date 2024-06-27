// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlackjackCard.h"
#include "CardDeck.h"
#include "BlackjackJoinWidget.h"
#include "CardDeck.h"
#include "BlackjackTable.generated.h"

DECLARE_MULTICAST_DELEGATE(FGameReady)

UCLASS()
class VICTIMS_API ABlackjackTable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABlackjackTable();
	
	UPROPERTY(EditAnywhere)
	class USceneComponent* RootComp;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* TableMeshComp;

	///플레이어위치 지정////////////////////////////////
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class USceneComponent*> PlayerLocs;
	
	///플레이어 지정을 위한 자리 콜리전////////////////////////////////
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class UCapsuleComponent*> PlayerCollisionLocs;

	// 카드 자리 지정을 위한 포지션
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class USceneComponent*> cardPositions;
	
	/////////자리 선정을 위한 오버랩 이벤트/////////
	UFUNCTION(BlueprintCallable)
	void BeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
	void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//게임 대기열에 들어간다, 게임에 참여하고 못움직이게 된다.
	UFUNCTION(BlueprintCallable)
	void ReadyToPlayGame(AActor* PlayerCharactor);

	UFUNCTION(BlueprintCallable)
	void PlayGame(TArray<class AActor*>PlayCharacterSet);

	UFUNCTION(BlueprintCallable)
	void BetMoney(TArray<class AActor*>PlayCharacterSet);

	UFUNCTION(BlueprintCallable)
	void EndGame(TArray<class AActor*>PlayCharacterSet);
	
	// Player Array
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AActor*>				PlayerArray;

	//현재 오버랩 되어있는 콜리전.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UPrimitiveComponent*> PlayerOverlapCollisiosArray;
	////////위젯/////////////////////////////////////
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBlackjackJoinWidget* joinWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> joinWidget_BP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ABlackjackPlyaer> BlackjackPlayer_BP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsPlayingnow=false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float readyTime=0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class AActor*>PlayerSet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ACardDeck> CardDeck_BP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bGameStartCountDown=false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ReadyDuration=3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class ACardDeck* deck;

	struct std::pair<FVector, FRotator > deckPoint;

	struct std::pair<FVector, FRotator > deckWeastPoint;

	////////////////////////////////////////////////////////////////////////////게임 스테이트/////////////////////////////////////////////////////////////////
	FGameReady gameReadyDelegate;


	//딜러카드
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class ABlackjackCard*> DealerCardSet;

	TArray<class ABlackjackPlyaer*> blackjackPlayerSet;

	
	//카드 받을지 말지
	bool bWnatMoreCard=true;
	//딜러 카드 합

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 dealerScoreSum;
	////////////////////1. 카드 두장씩 받기.////////////////////////////


	UFUNCTION(BlueprintCallable)
	void GetADealerCard();

	UFUNCTION(BlueprintCallable)
	void FlipADealerCard();

	UFUNCTION(BlueprintCallable)
	void CheckCard(ABlackjackCard* card);

	UFUNCTION(BlueprintCallable)
	void CalcScore(FString AddValue);

	UFUNCTION(BlueprintCallable)
	void ChooseAceValue(ABlackjackCard* AceCard);

	UFUNCTION(BlueprintCallable)
	void SetAceTo1(ABlackjackCard* card);

	UFUNCTION(BlueprintCallable)
	void SetAceTo11(ABlackjackCard* card);
	//////////////승 패 판단 
	UFUNCTION(BlueprintCallable)
	void Lose(ABlackjackPlyaer* player);

	UFUNCTION(BlueprintCallable)
	void PlayerBlackJack(ABlackjackPlyaer* player);

	UFUNCTION(BlueprintCallable)
	void PlayerStand(ABlackjackPlyaer* player, int32 playerScore);

	UFUNCTION(BlueprintCallable)
	void DealerCardOpen(ABlackjackPlyaer* player, int32 playerScore);
	///////////////////////게임이 끝나고/////////////////
	UFUNCTION(BlueprintCallable)
	void InitialiseGame();

	UFUNCTION(BlueprintCallable)
	void UnpossesPlayer();


	UFUNCTION(BlueprintCallable)
	void Win(ABlackjackPlyaer* player);

	UFUNCTION(BlueprintCallable)
	void Push(ABlackjackPlyaer* player);
	
	void SetWidget(class UBlajackMainWidget* widget);

	class UBlajackMainWidget* playerMainWidget = nullptr;
	


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
