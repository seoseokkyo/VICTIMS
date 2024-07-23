// Fill out your copyright notice in the Description page of Project Settings.


#include "Shelter.h"
#include "Net/UnrealNetwork.h"

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

void AShelter::SetPlayerName(FString ID)
{
    ServerRPC_SetPlayerName(ID);
}

void AShelter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AShelter, OwnerPlayerName);

}

void AShelter::ServerRPC_SetPlayerName_Implementation(const FString& ID)
{
    OwnerPlayerName = ID;
    
    NetMulticastRPC_SetPlayerName(ID);
}

void AShelter::NetMulticastRPC_SetPlayerName_Implementation(const FString& ID)
{
    OwnerPlayerName = ID;
}

void AShelter::ServerRPC_SetOriginPos_Implementation(FVector houseLocation)
{
    OriginPos = houseLocation;
}
