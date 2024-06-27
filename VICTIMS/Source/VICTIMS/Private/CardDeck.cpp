// Fill out your copyright notice in the Description page of Project Settings.


#include "CardDeck.h"
#include "BlackjackCard.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h>
#include <../../../../../../../Source/Runtime/Engine/Classes/Components/SceneComponent.h>

// Sets default values
ACardDeck::ACardDeck()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FString errorText;

	auto rootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("CardMesh"));
	SetRootComponent(rootComponent);
}

void ACardDeck::init_deck()
{
	//CardSet.Emplace();

	if (dt_cardInfoDataTable == nullptr)
		return;

	std::vector<int> temp = { Ace, 2, 3, 4, 5, 6, 7, 8, 9, 10, J, Q, K };

	if (CardSet.Num() > 0)
	{
		for (auto card : CardSet)
		{
			card->Destroy();
		}
	}

	CardSet.Reset(0);

	for (int i = 0; i < (int)ECardType::ECardTypeMax; i++)
	{
		// 0 : Clover , 1 : Heart, 2 : Diamond, 3 : Spade
		FActorSpawnParameters param;
		param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		param.TransformScaleMethod = ESpawnActorScaleMethod::OverrideRootScale;

		int cardValueTypeSize = temp.size();
		for (int j = 0; j < cardValueTypeSize; j++)
		{
			auto card = GetWorld()->SpawnActor<ABlackjackCard>(BP_CardFactory, GetActorLocation(), GetActorRotation(), param);

			card->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);

			card->cardInfo.cardType = (ECardType)i;

			card->SetActorRotation(FRotator(90.0f, 0.0f, 0.0f));

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

		bIsEmpty = false;
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


	for (int i = 0; i < size + 1; i++)
	{
		int randIndex = FMath::RandRange(0, size);
		CardSet[i]->SetActorRelativeLocation(FVector(0, 0, i * 0.1));
		CardSet[i]->SetActorRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
		CardSet[i]->SetActorRelativeScale3D(FVector(0.1, 0.1, 0.1));
	}
}

void ACardDeck::print_deck()
{
}

ABlackjackCard* ACardDeck::getCard()
{
	if (bIsEmpty)
		return nullptr;

	int lastIndex = CardSet.Num() - 1;

	ABlackjackCard* card = CardSet[lastIndex];
	CardSet.Remove(card);

	if (lastIndex == 0)
	{
		bIsEmpty = true;
	}

	return card;
}



bool ACardDeck::is_deck_empty()
{
	return bIsEmpty;
}

void ACardDeck::ResetDeck()
{
	CardSet.Empty();
	init_deck();
	shuffler();
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

