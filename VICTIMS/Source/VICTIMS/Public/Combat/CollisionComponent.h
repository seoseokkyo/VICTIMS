// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include <Engine/HitResult.h>
#include "CollisionComponent.generated.h"

class AVICTIMSCharacter;

DECLARE_DELEGATE_OneParam(FOnHitDeligate, FHitResult);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VICTIMS_API UCollisionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCollisionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	// Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UPrimitiveComponent* collisionMeshComponent;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Initialization")
	FName startSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Initialization")
	FName endSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Initialization")
	float traceRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Initialization")
	TArray<TEnumAsByte<EObjectTypeQuery>> collisionObjectTypes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Initialization")
	TEnumAsByte<EDrawDebugTrace::Type> drawDebugType;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Initialization")
	bool bCollisionEnabled = false;

	TArray<AActor*>		alreadyHitActors;
	TArray<AActor*>		actorsToIgnore;
	FHitResult			lastHitStruct;
	AActor*				hittedActor;

	FOnHitDeligate onHitDeligate;

	// Functions
	TArray<AActor*>			GetAlreadyHitActors() { return alreadyHitActors; };
	bool					CheckCollisionEnabled() { return bCollisionEnabled; };
	FHitResult				GetLastHit() { return lastHitStruct; };
	UPrimitiveComponent*	GetCollisionMeshComponent() { return collisionMeshComponent; };
	void					AddActorToIgnore(AActor* addActor);
	void					RemoveActorToIgnore(AActor* removeActor);
	TArray<AActor*>			GetActorsToIgnore() { return actorsToIgnore; };

	// FromCustomEvent
	UFUNCTION(BlueprintCallable)
	void SetCollisionMesh(UPrimitiveComponent* meshComp);
	UFUNCTION(BlueprintCallable)
	void SetEnableCollision(bool bEnable);
	UFUNCTION(BlueprintCallable)
	void ClearHitActor();
	UFUNCTION(BlueprintCallable)
	void CollisionTrace();
	UPROPERTY()
	AVICTIMSCharacter* ownerChar;
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Initialization")
	float daggerDamage=20.0f;
	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Initialization")
	float AxeDamage=40.0f;
	*/
	



};
