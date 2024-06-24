// Fill out your copyright notice in the Description page of Project Settings.


#include "CardGame.h"

// Sets default values
ACardGame::ACardGame()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACardGame::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACardGame::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

