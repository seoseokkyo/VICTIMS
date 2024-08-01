// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyManager.generated.h"

USTRUCT()
struct FEnemyManagingStruct
{
	GENERATED_BODY()

	class ANormalZombie* enemy = nullptr;
	FTransform originPos = FTransform::Identity;
	float dieTimer = 0.0f;
};


UCLASS()
class VICTIMS_API AEnemyManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnyWhere)
	TArray<FEnemyManagingStruct> enemys;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	TSubclassOf<class ANormalZombie> bp_Enemy;
};
