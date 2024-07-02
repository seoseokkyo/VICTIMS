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
		sightConfig->SightRadius = 500.F;
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
		hearingConfig->HearingRange = 3000.F;
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
}

void ANormalZombieController::Tick(float deltaTime)
{
	currentAttackDelayTime = FMath::Clamp(currentAttackDelayTime += deltaTime, 0, attackDelayTime);
	currentsightInitDelayTime = FMath::Clamp(currentsightInitDelayTime += deltaTime, 0, sightInitDelayTime);

	SetNoiseDetect(GetNoiseDetect() - deltaTime * 20);

	auto temp = GetPawn<ANormalZombie>();

	if (temp->motionState == ECharacterMotionState::Move)
	{
		float distance = FVector::Dist(temp->GetActorLocation(), targetActor->GetActorLocation());

		if (currentAttackDelayTime >= attackDelayTime)
		{
			if (distance < 100)
			{
				if (temp->motionState != ECharacterMotionState::Attack)
				{
					StopMovement();

					temp->PerformAttack(0, false);
				}

				currentAttackDelayTime = 0.0;

				bSightOn = false;

				bChaseStart = false;
			}
			else
			{
				FVector targetDir = targetActor->GetActorLocation() - temp->GetActorLocation();
				targetDir.Normalize();

				temp->SetActorRotation(targetDir.Rotation());

				//SetFocus(targetActor);

				MoveToLocation(targetActor->GetActorLocation());
			}
		}
		else
		{
			if (distance < 100)
			{
				StopMovement();

				temp->motionState = ECharacterMotionState::Idle;
			}
			else
			{
				FVector targetDir = targetActor->GetActorLocation() - temp->GetActorLocation();
				targetDir.Normalize();

				temp->SetActorRotation(targetDir.Rotation());

				//SetFocus(targetActor);

				MoveToLocation(targetActor->GetActorLocation());
			}
		}
	}

	bool bFindByHear = false;

	if (bSightOn)
	{
		if (temp->motionState == ECharacterMotionState::Idle)
		{
			temp->motionState = ECharacterMotionState::Move;

			temp->GetCharacterMovement()->MaxWalkSpeed = 280;

			if (targetActor)
			{
				FVector targetDir = targetActor->GetActorLocation() - temp->GetActorLocation();
				targetDir.Normalize();

				temp->SetActorRotation(targetDir.Rotation());

				//SetFocus(targetActor);

				MoveToLocation(targetActor->GetActorLocation());
			}
		}

		if (currentsightInitDelayTime >= sightInitDelayTime)
		{
			temp->motionState = ECharacterMotionState::Idle;

			bSightOn = false;

			bChaseStart = false;

			//SetFocus(nullptr);

			targetActor = nullptr;
		}
	}
	else
	{
		if (GetNoiseDetect() >= 300)
		{
			if (temp)
			{
				temp->GetCharacterMovement()->MaxWalkSpeed = 280;

				if (temp->motionState == ECharacterMotionState::Idle)
				{
					temp->motionState = ECharacterMotionState::Move;

					targetLocation = targetActor->GetActorLocation();

					bFindByHear = true;
				}
			}
		}
		else if (GetNoiseDetect() >= 200)
		{
			if (temp)
			{
				temp->GetCharacterMovement()->MaxWalkSpeed = 120;

				if (temp->motionState == ECharacterMotionState::Idle)
				{
					temp->motionState = ECharacterMotionState::Move;

					targetLocation = targetActor->GetActorLocation();

					bFindByHear = true;
				}
			}
		}
		else if (GetNoiseDetect() >= 100)
		{
			if (temp)
			{
				temp->GetCharacterMovement()->MaxWalkSpeed = 60;

				if (temp->motionState == ECharacterMotionState::Idle)
				{
					temp->motionState = ECharacterMotionState::Move;

					targetLocation = targetActor->GetActorLocation();

					bFindByHear = true;
				}
			}
		}
	}

	if (bFindByHear)
	{
		FVector targetDir = targetLocation - temp->GetActorLocation();
		targetDir.Normalize();

		temp->SetActorRotation(targetDir.Rotation());

		MoveToLocation(targetLocation);
	}
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
	if (Cast<AVICTIMSCharacter>(_Actor))
	{
		targetActor = _Actor;
	}
	else
	{
		return;
	}

	if (Stimulus.Type.Name == FName("Default__AISense_Sight"))
	{
		bSightOn = true;

		currentsightInitDelayTime = 0.0f;
	}
	else if (Stimulus.Type.Name == FName("Default__AISense_Hearing"))
	{
		SetNoiseDetect(GetNoiseDetect() + Stimulus.Strength);

		//UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Stimulus.Strength : %f"), hearingValue));
		//UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("GetNoiseDetect : %f"), GetNoiseDetect()));
	}
	//UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Stimulus.Type.Name : %s"), *Stimulus.Type.Name.ToString()));
}
