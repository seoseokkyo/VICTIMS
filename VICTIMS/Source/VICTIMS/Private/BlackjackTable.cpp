// Fill out your copyright notice in the Description page of Project Settings.


#include "BlackjackTable.h"
#include <../../../../../../../Source/Runtime/Core/Public/Delegates/Delegate.h>
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h>
#include "BlackjackJoinWidget.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/Components/PrimitiveComponent.h>
#include <../../../../../../../Source/Runtime/PhysicsCore/Public/CollisionShape.h>
#include <../../../../../../../Source/Runtime/Engine/Classes/Components/CapsuleComponent.h>
#include <BlackjackPlyaer.h>
#include "Player/VICTIMSCharacter.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/GameFramework/PlayerController.h>
#include <../../../../../../../Source/Runtime/Engine/Classes/Camera/CameraComponent.h>
#include <utility>
#include "CardDeck.h"
#include "Kismet/KismetMathLibrary.h"
#include "BlajackMainWidget.h"

// Sets default values
ABlackjackTable::ABlackjackTable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root Comp"));
	SetRootComponent(RootComp);

	TableMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TableMesh"));
	TableMeshComp->SetupAttachment(RootComp);
	TableMeshComp->SetRelativeScale3D(FVector(3, 3, 3));
	//7명의 플레이어 장소

	TArray<FVector> points;
	points.Add(FVector(153.501137, 21.004362, 0.000000));
	points.Add(FVector(126.666673, -45.333336, 0.000000));
	points.Add(FVector(69.333335, -85.333336, 0.000000));
	points.Add(FVector(3.156907, -106.142441, 0.000000));
	points.Add(FVector(-62.666668, -89.333336, 0.000000));
	points.Add(FVector(-121.333337, -54.666667, 0.000000));
	points.Add(FVector(-153.333338, 12.000000, 0.000000));

	for (int i = 0; i < points.Num(); i++)
	{
		auto playerPosSceneComp = CreateDefaultSubobject<USceneComponent>(*FString::Printf(TEXT("Player%dLocation"), i + 1));
		playerPosSceneComp->SetupAttachment(RootComp);
		playerPosSceneComp->SetRelativeLocation(points[i]);

		auto playerPosCapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(*FString::Printf(TEXT("Player%dCollision"), i + 1));
		playerPosCapsuleComp->SetupAttachment(playerPosSceneComp);
		playerPosCapsuleComp->SetRelativeScale3D(FVector(1, 1, 1.5));

		PlayerLocs.Add(playerPosSceneComp);
		PlayerCollisionLocs.Add(playerPosCapsuleComp);
	}

	for (int i = 0; i < points.Num(); i++)
	{
		PlayerArray.Add(nullptr);
	}

	//

	TArray<std::pair<FVector, FRotator>> cardPoints;
	cardPoints.Add(std::make_pair(FVector(30.341945, 10.521881, 5.050299) * 3, FRotator(0.0f, -15.0f, 0.0f)));
	cardPoints.Add(std::make_pair(FVector(24.035911, -1.739005, 5.050299) * 3, FRotator(0.0f, -40.0f, 0.0f)));
	cardPoints.Add(std::make_pair(FVector(13.009162, -9.797013, 5.050299) * 3, FRotator(0.0f, -60.0f, 0.0f)));
	cardPoints.Add(std::make_pair(FVector(0.073936, -12.977221, 5.050299) * 3, FRotator(0.0f, -90.0f, 0.0f)));
	cardPoints.Add(std::make_pair(FVector(-13.356078, -10.079165, 5.050299) * 3, FRotator(0.0f, -120.0f, 0.0f)));
	cardPoints.Add(std::make_pair(FVector(-24.170774, -1.950472, 5.050299) * 3, FRotator(0.0f, -140.0f, 0.0f)));
	cardPoints.Add(std::make_pair(FVector(-30.532360, 10.489962, 5.050299) * 3, FRotator(0.0f, -165.0f, 0.0f)));

	for (int i = 0; i < cardPoints.Num(); i++)
	{
		auto CardPointComp = CreateDefaultSubobject<USceneComponent>(*FString::Printf(TEXT("cardPoints%d"), i + 1));

		CardPointComp->SetupAttachment(RootComp);
		CardPointComp->SetRelativeLocation(cardPoints[i].first);
		CardPointComp->SetRelativeRotation(cardPoints[i].second);

		cardPositions.Add(CardPointComp);
	}
	//덱과 버리는 위치 지정
	deckPoint = std::make_pair(FVector(-41.75f, 71.99f, 14.77f), FRotator(0.0f, -113.30f, 0.0f));
	deckWeastPoint = std::make_pair(FVector(39.956839, 75.689062, 14.770671), FRotator(0.0f, -64.966086, 0.0f));
}
//////////////////////////오버랩 이벤트//////////////////////////
void ABlackjackTable::BeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("comp : %s"), *OverlappedComp->GetName()));

	if (bIsPlayingnow)
	{
		return;
	}

	if (joinWidget != nullptr)
	{
		int nums = PlayerLocs.Num();

		for (int i = 0; i < nums; i++)
		{
			if (OverlappedComp == PlayerCollisionLocs[i])
			{
				if (PlayerArray[i] == nullptr)
				{
					PlayerOverlapCollisiosArray.Add(OverlappedComp);
					if (!PlayerArray.Contains(OtherActor))
					{
						PlayerArray[i] = OtherActor;
						joinWidget->AddToPlayerScreen();
						GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);
					}

				}

				break;
			}
		}
	}
}

void ABlackjackTable::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == nullptr)
		return;

	PlayerOverlapCollisiosArray.Remove(OverlappedComponent);
	//현재 오버랩 되어있는 콜리전이 하나도 없다면,위젯을 지워라
	if (PlayerOverlapCollisiosArray.Num() == 0)
	{
		joinWidget->RemoveFromParent();
		GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(false);
	}

	int nums = PlayerLocs.Num();

	for (int i = 0; i < nums; i++)
	{
		if (OverlappedComponent == PlayerCollisionLocs[i])
		{
			if (PlayerArray[i] != nullptr)
			{
				PlayerArray[i] = nullptr;
			}

			break;
		}
	}
}


void ABlackjackTable::ReadyToPlayGame(AActor* PlayerCharactor)
{
	PlayerSet.Add(PlayerCharactor);
	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("OnReady")));
	bGameStartCountDown = true;

}
void ABlackjackTable::BetMoney(TArray<class AActor*> PlayCharacterSet)
{
	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("BetMoney")));
	PlayGame(PlayerSet);
}
void ABlackjackTable::PlayGame(TArray<class AActor*> PlayCharacterSet)
{
	joinWidget->RemoveFromParent();
	int32 playerNum = PlayerArray.Num();
	for (int i = 0; i < playerNum; i++)
	{
		if (PlayerArray[i] != nullptr)
		{
			auto playerCheck = Cast<AVICTIMSCharacter>(PlayerArray[i]);

			if (playerCheck != nullptr)
			{
				auto PlayerControllerCheck = Cast<APlayerController>(playerCheck->GetController());

				if (PlayerControllerCheck != nullptr)
				{
					auto spawnPawn = GetWorld()->SpawnActor<ABlackjackPlyaer>(BlackjackPlayer_BP);

					if (spawnPawn != nullptr)
					{
						FVector forVecTempXYZ = cardPositions[i]->GetComponentLocation() - PlayerLocs[i]->GetComponentLocation();
						FVector forVec = FVector(forVecTempXYZ.X, forVecTempXYZ.Y, 0);
						FRotator forwardRotator = UKismetMathLibrary::MakeRotFromZX(FVector(0, 0, 1), forVec);
						spawnPawn->FollowCamera->SetWorldRotation(forwardRotator);
						spawnPawn->FollowCamera->SetWorldLocation(cardPositions[i]->GetComponentLocation() + FVector(0, 0, 150));
						FVector dir = FVector(0, 0, -1);
						dir.Normalize();
						spawnPawn->FollowCamera->AddRelativeRotation(FRotator(-70, 0, 0));

						spawnPawn->table = this;
						spawnPawn->cardPosition = cardPositions[i];
						spawnPawn->originPlayer = playerCheck;

						PlayerControllerCheck->Possess(spawnPawn);

						blackjackPlayerSet.Add(spawnPawn);

						spawnPawn->GetCard();
						spawnPawn->GetCard();

						SetWidget(spawnPawn->MainWidget);

						//한장 받고
						GetADealerCard();
						//하나 까고
						FlipADealerCard();
						//하나는 받고 안깐다
						GetADealerCard();

					}
				}
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////카드 게임 스테이트/////////////////////////////////////////////////////
void ABlackjackTable::EndGame(TArray<class AActor*> PlayCharacterSet)
{


}



void ABlackjackTable::GetADealerCard()
{
	DealerCardSet.Add(deck->getCard());
	int cardNum = DealerCardSet.Num();
	DealerCardSet[cardNum - 1]->SetActorLocation(GetActorLocation() + FVector((cardNum - 1) * 5, (cardNum - 1) * 3, 15 + cardNum * 0.1) + GetActorRightVector() * 20);
}
void ABlackjackTable::FlipADealerCard()
{
	int LastCardNum = DealerCardSet.Num() - 1;
	DealerCardSet[LastCardNum]->Flip();
	CheckCard(DealerCardSet[LastCardNum]);
}


void ABlackjackTable::CheckCard(ABlackjackCard* card)
{
	FString cardValueTemp = card->cardInfo.cardValue;
	if (cardValueTemp == "J" || cardValueTemp == "Q" || cardValueTemp == "K")
	{
		cardValueTemp = "10";
		CalcScore(cardValueTemp);
	}
	else if (cardValueTemp == "A")
	{
		ChooseAceValue(card);
	}
	else
	{
		CalcScore(cardValueTemp);
	}
}

void ABlackjackTable::CalcScore(FString AddValue)
{
	int32 cardScore = FCString::Atoi(*AddValue);
	dealerScoreSum = dealerScoreSum + cardScore;
	if (dealerScoreSum == 21)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Dealer BlackJack!")));
	}

	else if (dealerScoreSum > 21)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("DealerScore : %iBust!"), dealerScoreSum), true, true, FColor::Cyan, 10);
	}
	else
	{
		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("DealerScore : %i "), dealerScoreSum), true, true, FColor::Cyan, 10);
	}

	if (playerMainWidget != nullptr)
	{
		if (playerMainWidget->dealerScoreDelegate.IsBound())
		{
			playerMainWidget->dealerScoreDelegate.Execute(dealerScoreSum);
		}
		else
		{
			UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("playerMainWidget->dealerScoreDelegate is not bound")));
		}
	}
}

void ABlackjackTable::ChooseAceValue(ABlackjackCard* AceCard)
{
	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("AceOn")));
	if (dealerScoreSum + 11 > 21)
	{
		SetAceTo1(AceCard);
	}
	else
	{
		SetAceTo11(AceCard);

	}
}

void ABlackjackTable::SetAceTo1(ABlackjackCard* card)
{
	card->cardInfo.cardValue = "1";
	CalcScore("1");
}

void ABlackjackTable::SetAceTo11(ABlackjackCard* card)
{
	card->cardInfo.cardValue = "11";
	CalcScore("11");
}

void ABlackjackTable::PlayerBlackJack(ABlackjackPlyaer* player)
{
	FlipADealerCard();

	if (dealerScoreSum == 21)
	{
		Push(player);
	}
	else
	{
		Win(player);
	}

}

void ABlackjackTable::PlayerStand(ABlackjackPlyaer* player, int32 playerScore)
{
	DealerCardOpen(player, playerScore);
}

void ABlackjackTable::DealerCardOpen(ABlackjackPlyaer* player, int32 playerScore)
{
	FlipADealerCard();

	if (dealerScoreSum < 17)
	{
		FTimerHandle timerHandle;
		GetWorld()->GetTimerManager().SetTimer(timerHandle, [&]() {

			GetADealerCard();
			DealerCardOpen(player, playerScore);


			}, 0.7f, false);

	}

	else if (dealerScoreSum == 21)
	{
		Lose(player);
	}

	else
	{
		if (playerScore > dealerScoreSum)
		{
			Win(player);
		}
		else if (playerScore < dealerScoreSum)
			if (dealerScoreSum > 21)
			{
				Win(player);
			}
			else
			{
				Lose(player);
			}
		else
		{
			Push(player);
		}
	}


}

void ABlackjackTable::InitialiseGame()
{

	FTimerHandle timerHandle;
	GetWorld()->GetTimerManager().SetTimer(timerHandle, [&]() {

		UnpossesPlayer();

		playerMainWidget->RemoveFromParent();
		//DealerCardSet.Empty();
		dealerScoreSum = 0;
		bIsPlayingnow = false;
		readyTime = 0;
		//PlayerArray.Empty();
		deck->init_deck();
		deck->shuffler();

		}, 2.0f, false);


}

void ABlackjackTable::UnpossesPlayer()
{
	int playerNum = blackjackPlayerSet.Num();
	for (int i = 0; i < playerNum; i++)
	{
		auto controller = blackjackPlayerSet[i]->GetController();

		if (controller)
		{
			controller->UnPossess();
			controller->Possess(blackjackPlayerSet[i]->originPlayer);
		}
	}
}

void ABlackjackTable::Win(ABlackjackPlyaer* player)
{

	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Win!")), true, true, FColor::Cyan, 10);
	InitialiseGame();
}

void ABlackjackTable::Push(ABlackjackPlyaer* player)
{

	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Push!")), true, true, FColor::Cyan, 10);
	InitialiseGame();
}

void ABlackjackTable::Lose(ABlackjackPlyaer* player)
{
	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Lose!")), true, true, FColor::Cyan, 10);
	InitialiseGame();
}

void ABlackjackTable::SetWidget(UBlajackMainWidget* widget)
{
	playerMainWidget = widget;
}





// Called when the game starts or when spawned
void ABlackjackTable::BeginPlay()
{
	Super::BeginPlay();


	joinWidget = CreateWidget<UBlackjackJoinWidget>(GetWorld(), joinWidget_BP);

	FActorSpawnParameters param;
	param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	param.TransformScaleMethod = ESpawnActorScaleMethod::OverrideRootScale;

	//시작 할 때 카드 덱을 정해진 장소에 스폰함.
	auto cardDeck = GetWorld()->SpawnActor<ACardDeck>(CardDeck_BP, param);
	cardDeck->SetActorLocation(GetActorLocation() + deckPoint.first);
	cardDeck->SetActorRotation(deckPoint.second);

	deck = cardDeck;
}

// Called every frame
void ABlackjackTable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsPlayingnow == false)
	{
		if (bGameStartCountDown)
		{
			int32 StartCountSecond = 0;
			int32 StartCountSecondTemp = 0;
			if (readyTime < ReadyDuration)
			{
				readyTime = readyTime + DeltaTime;
				StartCountSecondTemp = readyTime / 1;
				if (StartCountSecondTemp > StartCountSecond)
				{
					StartCountSecond = StartCountSecondTemp;
					UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Start After %f Later"), ReadyDuration - StartCountSecond));
				}
			}
			else
			{// 본격적인 게임 플레이로 넘겨주고, 변수도 바꿔준다.
				BetMoney(PlayerSet);
				bIsPlayingnow = true;
				readyTime = 0;
				StartCountSecond = 0;
				UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("OnPlay")));
				bGameStartCountDown = false;

			}
		}
	}



}

