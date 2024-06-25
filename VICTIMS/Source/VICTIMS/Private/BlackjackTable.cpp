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
	bGameStartCountDown=true;
	
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

						
						//spawnPawn->SetActorLocation(PlayerLocs[i]->GetComponentLocation() + FVector(0, 0, 150));
						FVector forVecTempXYZ=cardPositions[i]->GetComponentLocation()-PlayerLocs[i]->GetComponentLocation();
						FVector forVec=FVector(forVecTempXYZ.X, forVecTempXYZ.Y, 0);
						FRotator forwardRotator = UKismetMathLibrary::MakeRotFromZX(FVector(0,0,1), forVec);
						//FVector dir = RootComp->GetComponentLocation() - spawnPawn->FollowCamera->GetComponentLocation()-FVector(0,0,50);
						spawnPawn->FollowCamera->SetWorldRotation(forwardRotator);
						//spawnPawn->FollowCamera->SetWorldLocation(RootComp->GetComponentLocation()+FVector(0,0,150));
						spawnPawn->FollowCamera->SetWorldLocation(cardPositions[i]->GetComponentLocation() + FVector(0, 0, 150));
						//PlayerArray[i]->GetActorLocation();
						//PlayerCollisionLocs[i]->GetComponentLocation();
						FVector dir = FVector(0, 0, -1);
						dir.Normalize();
						//spawnPawn->FollowCamera->SetWorldRotation(dir.Rotation());
						spawnPawn->FollowCamera->AddRelativeRotation(FRotator(-70, 0, 0));

						PlayerControllerCheck->Possess(spawnPawn);

						PlayerControllerCheck->SetShowMouseCursor(false);
					}
				}
			}
		}
	}
}

void ABlackjackTable::EndGame(TArray<class AActor*> PlayCharacterSet)
{


}



// Called when the game starts or when spawned
void ABlackjackTable::BeginPlay()
{
	Super::BeginPlay();


	joinWidget = CreateWidget<UBlackjackJoinWidget>(GetWorld(), joinWidget_BP);

	FActorSpawnParameters param;
	param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	param.TransformScaleMethod = ESpawnActorScaleMethod::OverrideRootScale;

	for (int i = 0; i < cardPositions.Num(); i++)
	{
		auto cardDeck = GetWorld()->SpawnActor<ABlackjackCard>(CardDeck_BP, param);

		cardDeck->SetActorLocation(cardPositions[i]->GetComponentLocation());
		//cardDeck->AttachToComponent(cardPositions[i], FAttachmentTransformRules::SnapToTargetNotIncludingScale);

		//cardDeck->SetActorLocation();
	}
}

// Called every frame
void ABlackjackTable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsPlayingnow == false)
	{
		if (bGameStartCountDown)
		{
		int32 StartCountSecond=0;
		int32 StartCountSecondTemp=0;
			if (readyTime < ReadyDuration)
			{
				readyTime = readyTime + DeltaTime;
				StartCountSecondTemp=readyTime/1;
				if (StartCountSecondTemp>StartCountSecond)
				{
					StartCountSecond=StartCountSecondTemp;
					UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Start After %f Later"), ReadyDuration-StartCountSecond));
				}
			}
			else
			{// 본격적인 게임 플레이로 넘겨주고, 변수도 바꿔준다.
				PlayGame(PlayerSet);
				bIsPlayingnow = true;
				readyTime = 0;
				StartCountSecond=0;
				UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("OnPlay")));
				bGameStartCountDown=false;
				
			}
		}
	}

}

