// Fill out your copyright notice in the Description page of Project Settings.


#include "BlackjackCard.h"

// Sets default values
ABlackjackCard::ABlackjackCard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CardMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CardMesh"));
}

// Called when the game starts or when spawned
void ABlackjackCard::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABlackjackCard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

