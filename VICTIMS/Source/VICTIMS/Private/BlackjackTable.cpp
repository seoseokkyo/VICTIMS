// Fill out your copyright notice in the Description page of Project Settings.


#include "BlackjackTable.h"
#include <../../../../../../../Source/Runtime/Core/Public/Delegates/Delegate.h>
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h>
#include "BlackjackJoinWidget.h"

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
	Player1Loc = CreateDefaultSubobject<USceneComponent>(TEXT("Player1Location"));
	Player1Loc->SetupAttachment(RootComp);
	Player1Loc->SetRelativeLocation(FVector(153.501137, 21.004362, 0.000000));

	Player2Loc = CreateDefaultSubobject<USceneComponent>(TEXT("Player2Location"));
	Player2Loc->SetupAttachment(RootComp);
	Player2Loc->SetRelativeLocation(FVector(126.666673, -45.333336, 0.000000));

	Player3Loc = CreateDefaultSubobject<USceneComponent>(TEXT("Player3Location"));
	Player3Loc->SetupAttachment(RootComp);
	Player3Loc->SetRelativeLocation(FVector(69.333335, -85.333336, 0.000000));

	Player4Loc = CreateDefaultSubobject<USceneComponent>(TEXT("Player4Location"));
	Player4Loc->SetupAttachment(RootComp);
	Player4Loc->SetRelativeLocation(FVector(3.156907, -106.142441, 0.000000));

	Player5Loc = CreateDefaultSubobject<USceneComponent>(TEXT("Player5Location"));
	Player5Loc->SetupAttachment(RootComp);
	Player5Loc->SetRelativeLocation(FVector(-62.666668, -89.333336, 0.000000));

	Player6Loc = CreateDefaultSubobject<USceneComponent>(TEXT("Player6Location"));
	Player6Loc->SetupAttachment(RootComp);
	Player6Loc->SetRelativeLocation(FVector(-121.333337, -54.666667, 0.000000));

	Player7Loc = CreateDefaultSubobject<USceneComponent>(TEXT("Player7Location"));
	Player7Loc->SetupAttachment(RootComp);
	Player7Loc->SetRelativeLocation(FVector(-153.333338, 12.000000, 0.000000));

	//7명 플레이어 인터렉트 콜리전
	Player1Collision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Player1Collision"));
	Player1Collision->SetupAttachment(Player1Loc);
	Player1Collision->SetRelativeScale3D(FVector(0.2, 0.2, 1.5));
	Player1Collision->OnComponentBeginOverlap.AddDynamic(this, &ABlackjackTable::BeginOverlap);
	Player1Collision->OnComponentEndOverlap.AddDynamic(this, &ABlackjackTable::EndOverlap);


	Player2Collision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Player2Collision"));
	Player2Collision->SetupAttachment(Player2Loc);
	Player2Collision->SetRelativeScale3D(FVector(0.2, 0.2, 1.5));
	Player2Collision->OnComponentBeginOverlap.AddDynamic(this, &ABlackjackTable::BeginOverlap);
	Player2Collision->OnComponentEndOverlap.AddDynamic(this, &ABlackjackTable::EndOverlap);

	Player3Collision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Player3Collision"));
	Player3Collision->SetupAttachment(Player3Loc);
	Player3Collision->SetRelativeScale3D(FVector(0.2, 0.2, 1.5));
	Player3Collision->OnComponentBeginOverlap.AddDynamic(this, &ABlackjackTable::BeginOverlap);
	Player3Collision->OnComponentEndOverlap.AddDynamic(this, &ABlackjackTable::EndOverlap);

	Player4Collision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Player4Collision"));
	Player4Collision->SetupAttachment(Player4Loc);
	Player4Collision->SetRelativeScale3D(FVector(0.2, 0.2, 1.5));
	Player4Collision->OnComponentBeginOverlap.AddDynamic(this, &ABlackjackTable::BeginOverlap);
	Player4Collision->OnComponentEndOverlap.AddDynamic(this, &ABlackjackTable::EndOverlap);

	Player5Collision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Player5Collision"));
	Player5Collision->SetupAttachment(Player5Loc);
	Player5Collision->SetRelativeScale3D(FVector(0.2, 0.2, 1.5));
	Player5Collision->OnComponentBeginOverlap.AddDynamic(this, &ABlackjackTable::BeginOverlap);
	Player5Collision->OnComponentEndOverlap.AddDynamic(this, &ABlackjackTable::EndOverlap);

	Player6Collision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Player6Collision"));
	Player6Collision->SetupAttachment(Player6Loc);
	Player6Collision->SetRelativeScale3D(FVector(0.2, 0.2, 1.5));
	Player6Collision->OnComponentBeginOverlap.AddDynamic(this, &ABlackjackTable::BeginOverlap);
	Player6Collision->OnComponentEndOverlap.AddDynamic(this, &ABlackjackTable::EndOverlap);

	Player7Collision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Player7Collision"));
	Player7Collision->SetupAttachment(Player7Loc);
	Player7Collision->SetRelativeScale3D(FVector(0.2, 0.2, 1.5));
	Player7Collision->OnComponentBeginOverlap.AddDynamic(this, &ABlackjackTable::BeginOverlap);
	Player7Collision->OnComponentEndOverlap.AddDynamic(this, &ABlackjackTable::EndOverlap);



}
//////////////////////////오버랩 이벤트//////////////////////////
void ABlackjackTable::BeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("comp : %s"), *OverlappedComp->GetName()));
	
	if(joinWidget != nullptr)
	{
		

		if (OverlappedComp == Player1Collision)
		{
			//DrawDebugString(GetWorld(), GetActorLocation(), FString(TEXT("Press E to Join Player1")), OtherActor, FColor::Cyan, 3.0f, false, 1.5f);
			if (Player1==nullptr)
			{
				SetPlayer(OtherActor, 1);
				joinWidget->AddToViewport();
				GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);

			}
		}
		else if (OverlappedComp == Player2Collision)
		{
			//DrawDebugString(GetWorld(), GetActorLocation(), FString(TEXT("Press E to Join Player2")), OtherActor, FColor::Cyan, 3.0f, false, 1.5f);
			if (Player2 == nullptr)
			{
			SetPlayer(OtherActor, 2);
			joinWidget->AddToViewport();
			GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);
			}
		}
		else if (OverlappedComp == Player3Collision)
		{
			//DrawDebugString(GetWorld(), GetActorLocation(), FString(TEXT("Press E to Join Player3")), OtherActor, FColor::Cyan, 3.0f, false, 1.5f);
			if (Player3 == nullptr)
			{
			SetPlayer(OtherActor, 3);
			//UBlackjackJoinWidget->SetOwnerActor();
			joinWidget->AddToViewport();
			GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);
			}
		}
		else if (OverlappedComp == Player4Collision)
		{
			//DrawDebugString(GetWorld(), GetActorLocation(), FString(TEXT("Press E to Join Player4")), OtherActor, FColor::Cyan, 3.0f, false, 1.5f);
			if (Player4 == nullptr)
			{
			SetPlayer(OtherActor, 4);
			joinWidget->AddToViewport();
			GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);
			}
		}
		else if (OverlappedComp == Player5Collision)
		{
			//DrawDebugString(GetWorld(), GetActorLocation(), FString(TEXT("Press E to Join Player5")), OtherActor, FColor::Cyan, 3.0f, false, 1.5f);
			if (Player5 == nullptr)
			{
			SetPlayer(OtherActor, 5);
			joinWidget->AddToViewport();
			GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);
			}
		}
		else if (OverlappedComp == Player6Collision)
		{
			//DrawDebugString(GetWorld(), GetActorLocation(), FString(TEXT("Press E to Join Player6")), OtherActor, FColor::Cyan, 3.0f, false, 1.5f);
			if (Player6 == nullptr)
			{
			SetPlayer(OtherActor, 6);
			joinWidget->AddToViewport();
			GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);
			}
		}
		else if (OverlappedComp == Player7Collision)
		{
			//DrawDebugString(GetWorld(), GetActorLocation(), FString(TEXT("Press E to Join Player7")), OtherActor, FColor::Cyan, 3.0f, false, 1.5f);
			if (Player7 == nullptr)
			{
			SetPlayer(OtherActor, 7);
			joinWidget->AddToViewport();
			GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);
			}
		}
	}
}

void ABlackjackTable::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	joinWidget->RemoveFromParent();
	GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(false);
	
}

void ABlackjackTable::SetPlayer(AActor* Player, int32 playerNum)
{
	if (playerNum==1)
	{
		
		Player1=Player;
		
	}
	else if (playerNum == 2)
	{
		Player2 = Player;
	}
	else if (playerNum == 3)
	{
		Player3 = Player;
	}
	else if (playerNum == 4)
	{
		Player4 = Player;
	}
	else if (playerNum == 5)
	{
		Player5 = Player;
	}
	else if (playerNum == 6)
	{
		Player6 = Player;
	}
	else if (playerNum == 7)
	{
		Player7 = Player;
	}
	
}
void ABlackjackTable::SetPlayerNull(int32 playerNum)
{
	if (playerNum==1)
	{
		SetPlayer(nullptr, 1);
	}
	else if (playerNum == 2)
	{
		SetPlayer(nullptr, 2);
	}
	else if (playerNum == 3)
	{
		SetPlayer(nullptr, 3);
	}
	else if (playerNum == 4)
	{
		SetPlayer(nullptr, 4);
	}
	else if (playerNum == 5)
	{
		SetPlayer(nullptr, 5);
	}
	else if (playerNum == 6)
	{
		SetPlayer(nullptr, 6);
	}
	else if (playerNum == 7)
	{
		SetPlayer(nullptr, 7);
	}


}
void ABlackjackTable::PlayGame(AActor* PlayerCharactor)
{



}
// Called when the game starts or when spawned
void ABlackjackTable::BeginPlay()
{
	Super::BeginPlay();


	joinWidget = CreateWidget<UBlackjackJoinWidget>(GetWorld(), joinWidget_BP);
}

// Called every frame
void ABlackjackTable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

