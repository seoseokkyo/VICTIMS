// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LootActorManager.generated.h"

USTRUCT()
struct FLootActorStruct
{
	GENERATED_BODY()

	class ALootActor* lootActor = nullptr;
	float emptyTimer = 0.0f;
};


UCLASS()
class VICTIMS_API ALootActorManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALootActorManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
		
	float ownerCheckTimer = 0.0f;

	UPROPERTY(VisibleAnyWhere)
	TArray<FLootActorStruct> lootActors;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	TSubclassOf<class ALootActor> bp_LootActor;
};
