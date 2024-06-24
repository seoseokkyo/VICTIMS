// Fill out your copyright notice in the Description page of Project Settings.


#include "CardDeck.h"
#include "BlackjackCard.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h>

// Sets default values
ACardDeck::ACardDeck()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FString errorText;

}

void ACardDeck::init_deck()
{
	//CardSet.Emplace();

	if (dt_cardInfoDataTable == nullptr)
		return;

	std::vector<int> temp = { Ace, 2, 3, 4, 5, 6, 7, 8, 9, 10, J, Q, K };



	for (int i = 0; i < (int)ECardType::ECardTypeMax; i++)
	{
		// 0 : Clover , 1 : Heart, 2 : Diamond, 3 : Spade
		FActorSpawnParameters param;
		param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		param.TransformScaleMethod = ESpawnActorScaleMethod::OverrideRootScale;

		int cardValueTypeSize = temp.size();
		for (int j = 0; j < cardValueTypeSize; j++)
		{
			auto card = GetWorld()->SpawnActor<ABlackjackCard>(BP_CardFactory, param);
			card->cardInfo.cardType = (ECardType)i;

			if (j == 0)
			{
				card->cardInfo.cardValue = TEXT("A");
			}
			else if (j == 10)
			{
				card->cardInfo.cardValue = TEXT("J");
			}
			else if (j == 11)
			{
				card->cardInfo.cardValue = TEXT("Q");
			}
			else if (j == 12)
			{
				card->cardInfo.cardValue = TEXT("K");
			}
			else
			{
				card->cardInfo.cardValue = FString::Printf(TEXT("%d"), (j + 1));
			}



			int indexCheck = i * cardValueTypeSize + j;

			int iSizeCheck = CardInfos.Num();

			UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Selected Card Index : %d, CardInfos.Num : %d"), indexCheck, iSizeCheck));

			if (indexCheck < iSizeCheck)
			{
				card->CardMeshComp->SetStaticMesh(CardInfos[indexCheck]->cardMesh);
			}

			CardSet.Add(card);
		}

	}

}

void ACardDeck::shuffler()
{
	int size = CardSet.Num() - 1;
	for (int i = 0; i < size; i++)
	{
		int randIndex = FMath::RandRange(0, size);
		std::swap(CardSet[i], CardSet[randIndex]);
	}


	for (int i = 0; i < size; i++)
	{
		int randIndex = FMath::RandRange(0, size);
		CardSet[i]->SetActorLocation(FVector(50 + i * 50, 0, 300));
	}
}

void ACardDeck::print_deck()
{
}

int ACardDeck::getCard()
{
	return 0;
}

bool ACardDeck::is_deck_empty()
{
	return false;
}

void ACardDeck::BeginPlay()
{
	Super::BeginPlay();

	if (dt_cardInfoDataTable)
	{
		dt_cardInfoDataTable->GetAllRows<FBlackjackCardInfo>(TEXT("GetAllRows"), CardInfos);
	}

	init_deck();
	shuffler();

}

// Called every frame
void ACardDeck::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	

}

