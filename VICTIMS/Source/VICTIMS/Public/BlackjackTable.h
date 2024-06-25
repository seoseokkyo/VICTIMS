// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlackjackCard.h"
#include "CardDeck.h"
#include <../../../../../../../Source/Runtime/PhysicsCore/Public/CollisionShape.h>
#include <../../../../../../../Source/Runtime/Engine/Classes/Components/CapsuleComponent.h>
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
	UPROPERTY(EditAnywhere)
	class USceneComponent* Player1Loc;
	
	UPROPERTY(EditAnywhere)
	class USceneComponent* Player2Loc;

	UPROPERTY(EditAnywhere)
	class USceneComponent* Player3Loc;

	UPROPERTY(EditAnywhere)
	class USceneComponent* Player4Loc;

	UPROPERTY(EditAnywhere)
	class USceneComponent* Player5Loc;

	UPROPERTY(EditAnywhere)
	class USceneComponent* Player6Loc;

	UPROPERTY(EditAnywhere)
	class USceneComponent* Player7Loc;
	///�÷��̾� ������ ���� �ڸ� �ݸ���////////////////////////////////
	UPROPERTY(EditAnywhere)
	class UCapsuleComponent* Player1Collision;

	UPROPERTY(EditAnywhere)
	class UCapsuleComponent* Player2Collision;

	UPROPERTY(EditAnywhere)
	class UCapsuleComponent* Player3Collision;

	UPROPERTY(EditAnywhere)
	class UCapsuleComponent* Player4Collision;

	UPROPERTY(EditAnywhere)
	class UCapsuleComponent* Player5Collision;

	UPROPERTY(EditAnywhere)
	class UCapsuleComponent* Player6Collision;

	UPROPERTY(EditAnywhere)
	class UCapsuleComponent* Player7Collision;

	/////////�ڸ� ������ ���� ������ �̺�Ʈ/////////
	UFUNCTION(BlueprintCallable)
	void BeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
	void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//������ �� �÷��̾ �ش� ��ҿ� �ش��ϴ� ��ȣ�� �÷��̾�� �������ִ� �Լ�
	UFUNCTION(BlueprintCallable)
	void SetPlayer(AActor* Player, int32 playerNum);

	//�÷��̾ ������ �ٽ� �������ͷ�
	UFUNCTION(BlueprintCallable)
	void SetPlayerNull(int32 playerNum);

	//�÷��� ������ ������, ���ӿ� �����ϰ� �������̰� �ȴ�.
	UFUNCTION(BlueprintCallable)
	void PlayGame(AActor* PlayerCharactor);
	

	///�÷��̾�1~7��////////////////////////////////
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* Player1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* Player2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* Player3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* Player4;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* Player5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* Player6;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* Player7;
	/////////////////////////////////////////////////
	////////����/////////////////////////////////////
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBlackjackJoinWidget* joinWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> joinWidget_BP;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
