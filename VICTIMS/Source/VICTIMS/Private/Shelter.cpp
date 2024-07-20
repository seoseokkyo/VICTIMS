// Fill out your copyright notice in the Description page of Project Settings.


#include "Shelter.h"

//AShelter::AShelter()
//{
//    PrimaryActorTick.bCanEverTick = true;
//
//    // √ ±‚»≠
//    HouseNumber = -1;
//}

void AShelter::BeginPlay()
{
    Super::BeginPlay();
}

void AShelter::Tick(float DeltaTime)
{   
    Super::Tick(DeltaTime);
}

void AShelter::ServerRPC_SetOriginPos_Implementation(FVector houseLocation)
{
    OriginPos = houseLocation;
}
