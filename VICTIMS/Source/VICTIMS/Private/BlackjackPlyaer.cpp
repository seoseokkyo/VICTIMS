// Fill out your copyright notice in the Description page of Project Settings.


#include "BlackjackPlyaer.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/Camera/CameraComponent.h>
#include <../../../../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputSubsystems.h>
#include <../../../../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputComponent.h>
#include "BlackjackTable.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/KismetMathLibrary.h>
#include <../../../../../../../Source/Runtime/Engine/Classes/Components/SceneComponent.h>
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h>

// Sets default values
ABlackjackPlyaer::ABlackjackPlyaer()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	//FollowCamera->SetupAttachment(RootComponent);
	FollowCamera->SetRelativeLocation(FVector(0, 0, 150) + GetActorLocation());
}

// Called when the game starts or when spawned
void ABlackjackPlyaer::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ABlackjackPlyaer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABlackjackPlyaer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABlackjackPlyaer::Look);
		EnhancedInputComponent->BindAction(LeftClickAction, ETriggerEvent::Started, this, &ABlackjackPlyaer::LeftClickFunction);
		
	}
}


void ABlackjackPlyaer::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	FollowCamera->AddRelativeRotation(FRotator(-LookAxisVector.Y, LookAxisVector.X, 0));
}

void ABlackjackPlyaer::LeftClickFunction(const FInputActionValue& Value)
{
	GetCard();
}

void ABlackjackPlyaer::GetCard()
{
	PlayerCardSet.Add(table->deck->getCard());

	int cardNum = PlayerCardSet.Num();
	PlayerCardSet[cardNum - 1]->
		SetActorLocation(cardPosition->GetComponentLocation() + FVector((cardNum - 1) * 5, (cardNum - 1) * 3, 0.1 + cardNum * 0.1));

	PlayerCardSet[cardNum - 1]->SetActorRotation(UKismetMathLibrary::MakeRotFromXZ(FVector(0, 0, 1), -1 * cardPosition->GetForwardVector()));
	PlayerCardSet[cardNum - 1]->Flip();

	CalcScore();
}

void ABlackjackPlyaer::CalcScore()
{
int cardNum = PlayerCardSet.Num();
FString cardValueTemp= PlayerCardSet[cardNum - 1]->cardInfo.cardValue;
	if (cardValueTemp=="J"|| cardValueTemp == "Q"|| cardValueTemp == "K")
	{
		cardValueTemp = "10";
	}
	else if (cardValueTemp == "A")
	{
		cardValueTemp = "11";
	}

	int32 cardScore = FCString::Atoi(*cardValueTemp);
	
	scoreSum=scoreSum+cardScore;
	if (scoreSum==21)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Player BlackJack!")));
	}

	else if (scoreSum > 21)
	{

		for (int i=0; i<cardNum; i++)
		{
			if (PlayerCardSet[i]->cardInfo.cardValue == "A")
				{
				PlayerCardSet[i]->cardInfo.cardValue = "1";
				scoreSum = 0;
					for (int j = 0; j < cardNum; j++)
					{
						scoreSum = scoreSum + FCString::Atoi(*PlayerCardSet[j]->cardInfo.cardValue);
					}

					if (scoreSum == 21)
					{
						UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Player BlackJack!")));
					}

					else if (scoreSum > 21)
					{
						UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Bust!")), true, true, FColor::Cyan, 10);
					}

					else
					{
						UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("MyScore : %i"), scoreSum), true, true, FColor::Cyan, 10);
					}

				}
			else
			{
				UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Bust!")), true, true, FColor::Cyan, 10);
			}
		}
	}
	
	else
	{
		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("MyScore : %i"), scoreSum), true, true, FColor::Cyan, 10);
	}
	
}
