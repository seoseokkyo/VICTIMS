// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlackjackCard.h"
#include "CardDeck.h"
#include "BlackjackJoinWidget.h"
#include "BlackjackTable.generated.h"

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
	
	// Player Array
	TArray<AActor*>				PlayerArray;

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
	TSubclassOf<class ABlackjackCard> CardDeck_BP;



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
