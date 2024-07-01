// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "NormalZombieController.generated.h"

/**
 * 
 */
UCLASS()
class VICTIMS_API ANormalZombieController : public AAIController
{
	GENERATED_BODY()
	
public:
	ANormalZombieController();

	UFUNCTION()
	virtual void BeginPlay() override;

	// Perception
	UPROPERTY(EditAnywhere, Category = "MySettings")
	class ANormalZombieController* aiController;

	UPROPERTY(EditAnywhere, Category = "MySettings")
	class UAIPerceptionComponent* aiPerceptionComp;

	// AI Sight Config
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MySettings")
	class UAISenseConfig_Sight* sightConfig;

	// AI Hearing Config
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MySettings")
	class UAISenseConfig_Hearing* hearingConfig;

	UFUNCTION()
	void SetupPerceptionSystem();

	UFUNCTION()
	void OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus);
	//	
};
