// Fill out your copyright notice in the Description page of Project Settings.


#include "NormalZombieController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include <Kismet/GameplayStatics.h>

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
		sightConfig->PeripheralVisionAngleDegrees = 90.F;
		sightConfig->SetMaxAge(5.F); // seconds - perceived stimulus forgotten after this time
		sightConfig->AutoSuccessRangeFromLastSeenLocation = 520.F;
		sightConfig->DetectionByAffiliation.bDetectEnemies = true;
		sightConfig->DetectionByAffiliation.bDetectFriendlies = true;
		sightConfig->DetectionByAffiliation.bDetectNeutrals = true;

		aiPerceptionComp->SetDominantSense(*sightConfig->GetSenseImplementation());

		aiPerceptionComp->ConfigureSense(*sightConfig);
	}

	if (hearingConfig != nullptr) {
		hearingConfig->HearingRange = 10000.F;
		hearingConfig->SetMaxAge(5.F);
		hearingConfig->DetectionByAffiliation.bDetectEnemies = true;
		hearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
		hearingConfig->DetectionByAffiliation.bDetectNeutrals = true;

		aiPerceptionComp->ConfigureSense(*hearingConfig);
	}
}

void ANormalZombieController::BeginPlay()
{
	Super::BeginPlay();

	SetupPerceptionSystem();
}

void ANormalZombieController::SetupPerceptionSystem()
{
	if (sightConfig != nullptr || hearingConfig != nullptr)
	{
		aiPerceptionComp->OnTargetPerceptionUpdated.AddDynamic(
			this, &ANormalZombieController::OnTargetDetected);
	}
}

void ANormalZombieController::OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus)
{
	if (Stimulus.Type.Name == FName(""))
	{

	}
	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Detected : %s"), *Actor->GetActorNameOrLabel()));
}
