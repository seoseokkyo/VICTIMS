// Fill out your copyright notice in the Description page of Project Settings.


#include "BlackjackTable.h"

// Sets default values
ABlackjackTable::ABlackjackTable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	TableMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TableMesh"));



}

// Called when the game starts or when spawned
void ABlackjackTable::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABlackjackTable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

