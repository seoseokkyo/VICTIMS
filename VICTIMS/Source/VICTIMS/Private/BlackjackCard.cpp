// Fill out your copyright notice in the Description page of Project Settings.


#include "BlackjackCard.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h>

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

	if(bOnFlip ==true)
	{
	
	if (RotTemp<180)
	{
		RotTemp=RotTemp+DeltaTime*300; 
		AddActorLocalRotation(FRotator(0, DeltaTime * 300, 0));
	}
		
	else
	{
		RotTemp=0;
		bOnFlip=false;
	}
	}


}

void ABlackjackCard::Flip()
{
	
		bOnFlip = true;
	
}

