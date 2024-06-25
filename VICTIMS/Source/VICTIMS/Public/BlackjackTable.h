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
	///플레이어위치 지정////////////////////////////////
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
	///플레이어 지정을 위한 자리 콜리전////////////////////////////////
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

	/////////자리 선정을 위한 오버랩 이벤트/////////
	UFUNCTION(BlueprintCallable)
	void BeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
	void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//오버랩 된 플레이어를 해당 장소에 해당하는 번호의 플레이어로 지정해주는 함수
	UFUNCTION(BlueprintCallable)
	void SetPlayer(AActor* Player, int32 playerNum);

	//플레이어가 떠나면 다시 널포인터로
	UFUNCTION(BlueprintCallable)
	void SetPlayerNull(int32 playerNum);

	//플레이 게임을 누르면, 게임에 참여하고 못움직이게 된다.
	UFUNCTION(BlueprintCallable)
	void PlayGame(AActor* PlayerCharactor);
	

	///플레이어1~7번////////////////////////////////
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
	////////위젯/////////////////////////////////////
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
