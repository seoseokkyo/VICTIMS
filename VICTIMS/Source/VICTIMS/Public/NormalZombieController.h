// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include <atomic>
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

	UFUNCTION()
	virtual void Tick(float deltaTime) override;

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

	std::atomic<float> noiseDetect = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MySettings")
	AActor* targetActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MySettings")
	FVector targetLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MySettings")
	bool bSightOn = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MySettings")
	bool bChaseStart = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MySettings")
	float attackDelayTime = 5.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MySettings")
	float currentAttackDelayTime = 5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MySettings")
	float sightInitDelayTime = 20.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MySettings")
	float currentsightInitDelayTime = 0.0f;

	UFUNCTION()
	void SetupPerceptionSystem();

	UFUNCTION()
	void OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus);
	//	

	UFUNCTION()
	void SetNoiseDetect(float value) { noiseDetect.store(FMath::Clamp(value, 0, 1000)); };

	UFUNCTION()
	float GetNoiseDetect() { return noiseDetect.load(); };
};
