// Fill out your copyright notice in the Description page of Project Settings.


#include "NormalZombieController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include <Kismet/GameplayStatics.h>
#include "NormalZombie.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "VICTIMSCharacter.h"

ANormalZombieController::ANormalZombieController()
{

	aiPerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Ai Perception Comp"));
	//aiController->SetPerceptionComponent(*aiPerceptionComp);

	// AI Sight Config
	sightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("AI Sight Config"));

	// AI Hearing Config
	hearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("AI Hearing Config"));

	if (sightConfig != nullptr) {
		sightConfig->SightRadius = 700.F;
		sightConfig->LoseSightRadius = sightConfig->SightRadius + 25.F;
		sightConfig->PeripheralVisionAngleDegrees = 45.F;
		sightConfig->SetMaxAge(5.F); // seconds - perceived stimulus forgotten after this time
		sightConfig->AutoSuccessRangeFromLastSeenLocation = 520.F;
		sightConfig->DetectionByAffiliation.bDetectEnemies = true;
		sightConfig->DetectionByAffiliation.bDetectFriendlies = true;
		sightConfig->DetectionByAffiliation.bDetectNeutrals = true;

		aiPerceptionComp->ConfigureSense(*sightConfig);
	}

	if (hearingConfig != nullptr) {
		hearingConfig->HearingRange = 10000.F;
		hearingConfig->SetMaxAge(5.F);
		hearingConfig->DetectionByAffiliation.bDetectEnemies = true;
		hearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
		hearingConfig->DetectionByAffiliation.bDetectNeutrals = true;

		aiPerceptionComp->SetDominantSense(*hearingConfig->GetSenseImplementation());

		aiPerceptionComp->ConfigureSense(*hearingConfig);
	}
}

void ANormalZombieController::BeginPlay()
{
	Super::BeginPlay();

	SetupPerceptionSystem();

	onceMovingLimitTime = FMath::RandRange(1, 3);

	bSightOn = false;
}

void ANormalZombieController::Tick(float deltaTime)
{
	auto temp = GetPawn<ANormalZombie>();

	if (temp == nullptr || targetActor == nullptr)
	{
		return;
	}

	if (temp->bDead == true)
	{
		return;
	}

	if (temp->motionState == ECharacterMotionState::Hit || temp->motionState == ECharacterMotionState::Attack)
	{
		StopMovement();

		bMoveStart = false;
		bMoveAtLocation = false;
	}

	currentAttackDelayTime = FMath::Clamp(currentAttackDelayTime += deltaTime, 0, attackDelayTime);
	currentMovingTime = FMath::Clamp(currentMovingTime += deltaTime, 0, onceMovingLimitTime);

	SetNoiseDetect(GetNoiseDetect() - deltaTime * 20);

	auto myLoc = temp->GetActorLocation();

	if (FVector::Dist(myLoc, targetLocation) < 150 || currentMovingTime >= onceMovingLimitTime)
	{
		if (temp->motionState == ECharacterMotionState::Move)
		{
			temp->motionState = ECharacterMotionState::Idle;
			currentMovingTime = 0.0f;

			onceMovingLimitTime = FMath::RandRange(1, 3);

			bMoveStart = false;
			bMoveAtLocation = false;
		}
	}

	bool bNoiseDetect = false;

	if (bSightOn)
	{
		currentsightInitDelayTime = FMath::Clamp(currentsightInitDelayTime += deltaTime, 0, sightInitDelayTime);

		if (currentsightInitDelayTime >= sightInitDelayTime)
		{
			bSightOn = false;
		}
	}

	if (bSightOn)
	{
		float distance = FVector::Dist(myLoc, targetActor->GetActorLocation());

		temp->GetCharacterMovement()->MaxWalkSpeed = 400;

		if (currentAttackDelayTime >= attackDelayTime)
		{
			if (distance < 100)
			{
				if (temp->motionState != ECharacterMotionState::Attack)
				{
					temp->PerformAttack(0, false);

					currentAttackDelayTime = 0.0;

					bSightOn = false;
				}
			}
		}
	}
	else
	{
		if (GetNoiseDetect() >= 800)
		{
			temp->GetCharacterMovement()->MaxWalkSpeed = 280;

			bNoiseDetect = true;
		}
		else if (GetNoiseDetect() >= 300)
		{
			temp->GetCharacterMovement()->MaxWalkSpeed = 180;

			bNoiseDetect = true;
		}
		else if (GetNoiseDetect() >= 100)
		{
			temp->GetCharacterMovement()->MaxWalkSpeed = 80;

			bNoiseDetect = true;
		}
	}

	if ((bSightOn || bNoiseDetect) && temp->motionState == ECharacterMotionState::Idle && bMoveStart == false)
	{
		temp->motionState = ECharacterMotionState::Move;

		if (bSightOn)
		{
			if (bMoveAtLocation)
			{
				StopMovement();
				bMoveAtLocation = false;
			}

			MoveToActor(targetActor, 50);
		}
		else
		{
			MoveToLocation(targetLocation, 100);

			bMoveAtLocation = true;
		}

		bMoveStart = true;
	}

	//if (temp->motionState == ECharacterMotionState::Move)
	//{
	//	float distance = FVector::Dist(temp->GetActorLocation(), targetActor->GetActorLocation());

	//	if (currentAttackDelayTime >= attackDelayTime)
	//	{
	//		if (distance < 100)
	//		{
	//			if (temp->motionState != ECharacterMotionState::Attack)
	//			{
	//				StopMovement();

	//				temp->PerformAttack(0, false);
	//			}

	//			currentAttackDelayTime = 0.0;

	//			bSightOn = false;

	//			bChaseStart = false;
	//		}
	//		else
	//		{
	//			FVector targetDir = targetActor->GetActorLocation() - temp->GetActorLocation();
	//			targetDir.Normalize();

	//			temp->SetActorRotation(targetDir.Rotation());

	//			//SetFocus(targetActor);

	//			MoveToLocation(targetActor->GetActorLocation());
	//		}
	//	}
	//	else
	//	{
	//		if (distance < 100)
	//		{
	//			StopMovement();

	//			temp->motionState = ECharacterMotionState::Idle;
	//		}
	//		else
	//		{
	//			FVector targetDir = targetActor->GetActorLocation() - temp->GetActorLocation();
	//			targetDir.Normalize();

	//			temp->SetActorRotation(targetDir.Rotation());

	//			//SetFocus(targetActor);

	//			MoveToLocation(targetActor->GetActorLocation());
	//		}
	//	}
	//}

	//bool bFindByHear = false;

	//if (bSightOn)
	//{
	//	if (temp->motionState == ECharacterMotionState::Idle)
	//	{
	//		temp->motionState = ECharacterMotionState::Move;

	//		temp->GetCharacterMovement()->MaxWalkSpeed = 280;

	//		if (targetActor)
	//		{
	//			FVector targetDir = targetActor->GetActorLocation() - temp->GetActorLocation();
	//			targetDir.Normalize();

	//			temp->SetActorRotation(targetDir.Rotation());

	//			//SetFocus(targetActor);

	//			MoveToLocation(targetActor->GetActorLocation());
	//		}
	//	}

	//	if (currentsightInitDelayTime >= sightInitDelayTime)
	//	{
	//		temp->motionState = ECharacterMotionState::Idle;

	//		bSightOn = false;

	//		bChaseStart = false;

	//		//SetFocus(nullptr);

	//		targetActor = nullptr;
	//	}
	//}
	//else
	//{
	//	if (GetNoiseDetect() >= 300)
	//	{
	//		if (temp)
	//		{
	//			temp->GetCharacterMovement()->MaxWalkSpeed = 280;

	//			if (temp->motionState == ECharacterMotionState::Idle)
	//			{
	//				temp->motionState = ECharacterMotionState::Move;

	//				targetLocation = targetActor->GetActorLocation();

	//				bFindByHear = true;
	//			}
	//		}
	//	}
	//	else if (GetNoiseDetect() >= 200)
	//	{
	//		if (temp)
	//		{
	//			temp->GetCharacterMovement()->MaxWalkSpeed = 120;

	//			if (temp->motionState == ECharacterMotionState::Idle)
	//			{
	//				temp->motionState = ECharacterMotionState::Move;

	//				targetLocation = targetActor->GetActorLocation();

	//				bFindByHear = true;
	//			}
	//		}
	//	}
	//	else if (GetNoiseDetect() >= 100)
	//	{
	//		if (temp)
	//		{
	//			temp->GetCharacterMovement()->MaxWalkSpeed = 60;

	//			if (temp->motionState == ECharacterMotionState::Idle)
	//			{
	//				temp->motionState = ECharacterMotionState::Move;

	//				targetLocation = targetActor->GetActorLocation();

	//				bFindByHear = true;
	//			}
	//		}
	//	}
	//}

	//if (bFindByHear)
	//{
	//	FVector targetDir = targetLocation - temp->GetActorLocation();
	//	targetDir.Normalize();

	//	temp->SetActorRotation(targetDir.Rotation());

	//	MoveToLocation(targetLocation);
	//}
}

void ANormalZombieController::SetupPerceptionSystem()
{
	if (sightConfig != nullptr || hearingConfig != nullptr)
	{
		aiPerceptionComp->OnTargetPerceptionUpdated.AddDynamic(
			this, &ANormalZombieController::OnTargetDetected);
	}
}

void ANormalZombieController::OnTargetDetected(AActor* _Actor, FAIStimulus const Stimulus)
{
	// Enemy : Enemy Check
	FString ownerTeamTag;
	FString hitActorTeamTag;

	bool bOwnerTagFind = false;
	for (auto tag : GetPawn()->Tags)
	{
		if (tag.ToString().Contains("Team"))
		{
			ownerTeamTag = tag.ToString();
			bOwnerTagFind = true;
			break;
		}
	}

	bool bHitActorTagFind = false;
	for (auto tag : _Actor->Tags)
	{
		if (tag.ToString().Contains("Team"))
		{
			hitActorTeamTag = tag.ToString();
			bHitActorTagFind = true;
			break;
		}
	}

	if (bOwnerTagFind && bHitActorTagFind && (ownerTeamTag == hitActorTeamTag))
	{
		return;
	}

	if (auto charCheck = Cast<ACharacterBase>(_Actor))
	{
		if (false == charCheck->bDead)
		{
			targetActor = _Actor;
		}
		else
		{
			targetActor = nullptr;
			return;
		}
	}


	if (Stimulus.Type.Name == FName("Default__AISense_Sight"))
	{
		bSightOn = true;

		currentsightInitDelayTime = 0.0f;
	}
	else if (Stimulus.Type.Name == FName("Default__AISense_Hearing"))
	{
		if (false == Stimulus.IsValid())
		{
			return;
		}

		// 1000보다 먼 거리에서 난 20보다 작은 소리는 무시
		if (Stimulus.Strength < 20)
		{
			if (Stimulus.StimulusLocation == FVector::ZeroVector || GetPawn()->GetActorLocation() == FVector::ZeroVector)
			{
				return;
			}

			if (FVector::Dist(Stimulus.StimulusLocation, GetPawn()->GetActorLocation()) > 1000.0f)
			{
				return;
			}
		}
		else if (Stimulus.Strength < 100)
		{
			if (FVector::Dist(Stimulus.StimulusLocation, GetPawn()->GetActorLocation()) > 1500.0f)
			{
				return;
			}

			SetNoiseDetect(GetNoiseDetect() + Stimulus.Strength);

			SetTargetLocation(Stimulus.StimulusLocation);
		}
		else
		{
			//UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Dist : %f"), FVector::Dist(Stimulus.StimulusLocation, GetPawn()->GetActorLocation())));

			SetNoiseDetect(GetNoiseDetect() + Stimulus.Strength);

			SetTargetLocation(Stimulus.StimulusLocation);
		}

		//UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Stimulus.Strength : %f"), Stimulus.Strength));
		//UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("GetNoiseDetect : %f"), GetNoiseDetect()));
	}
	//UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Stimulus.Type.Name : %s"), *Stimulus.Type.Name.ToString()));
}

void ANormalZombieController::SetNoiseDetect(float value)
{
	ServerRPC_SetNoiseDetect(value);
}

void ANormalZombieController::SetTargetLocation(FVector loc)
{
	ServerRPC_SetTargetLocation(loc);
}

void ANormalZombieController::ServerRPC_SetTargetLocation_Implementation(FVector loc)
{
	targetLocation = loc;

	NetMulticastRPC_SetTargetLocation(targetLocation);
}

void ANormalZombieController::NetMulticastRPC_SetTargetLocation_Implementation(FVector loc)
{
	targetLocation = loc;
}

void ANormalZombieController::ServerRPC_SetNoiseDetect_Implementation(float value)
{
	noiseDetect.store(FMath::Clamp(value, 0, 1000));

	NetMulticastRPC_SetNoiseDetect(noiseDetect.load());
}

void ANormalZombieController::NetMulticastRPC_SetNoiseDetect_Implementation(float value)
{
	noiseDetect.store(FMath::Clamp(value, 0, 1000));
}
