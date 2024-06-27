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

	///�÷��̾���ġ ����////////////////////////////////
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class USceneComponent*> PlayerLocs;
	
	///�÷��̾� ������ ���� �ڸ� �ݸ���////////////////////////////////
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class UCapsuleComponent*> PlayerCollisionLocs;

	// ī�� �ڸ� ������ ���� ������
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class USceneComponent*> cardPositions;
	
	/////////�ڸ� ������ ���� ������ �̺�Ʈ/////////
	UFUNCTION(BlueprintCallable)
	void BeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
	void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//���� ��⿭�� ����, ���ӿ� �����ϰ� �������̰� �ȴ�.
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

	//���� ������ �Ǿ��ִ� �ݸ���.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UPrimitiveComponent*> PlayerOverlapCollisiosArray;
	////////����/////////////////////////////////////
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

	////////////////////////////////////////////////////////////////////////////���� ������Ʈ/////////////////////////////////////////////////////////////////
	FGameReady gameReadyDelegate;


	//����ī��
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class ABlackjackCard*> DealerCardSet;

	TArray<class ABlackjackPlyaer*> blackjackPlayerSet;

	
	//ī�� ������ ����
	bool bWnatMoreCard=true;
	//���� ī�� ��

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 dealerScoreSum;
	////////////////////1. ī�� ���徿 �ޱ�.////////////////////////////


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
	//////////////�� �� �Ǵ� 
	UFUNCTION(BlueprintCallable)
	void Lose(ABlackjackPlyaer* player);

	UFUNCTION(BlueprintCallable)
	void PlayerBlackJack(ABlackjackPlyaer* player);

	UFUNCTION(BlueprintCallable)
	void PlayerStand(ABlackjackPlyaer* player, int32 playerScore);

	UFUNCTION(BlueprintCallable)
	void DealerCardOpen(ABlackjackPlyaer* player, int32 playerScore);
	///////////////////////������ ������/////////////////
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
